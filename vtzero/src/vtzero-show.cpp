
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <getopt.h>

#include <vtzero/reader.hpp>

#include "utils.hpp"

struct geom_handler_points {

    void point(const vtzero::point& point) const {
        std::cout << "POINT(" << point.x << ',' << point.y << ")\n";
    }

};

struct geom_handler_linestrings {

    std::string output;

    void linestring_begin() {
        output = "LINESTRING(";
    }

    void linestring_point(const vtzero::point& point) {
        output += std::to_string(point.x);
        output += ' ';
        output += std::to_string(point.y);
        output += ',';
    }

    void linestring_end() {
        if (output.empty()) {
            return;
        }
        if (output.back() == ',') {
            output.resize(output.size() - 1);
        }
        output += ")\n";
        std::cout << output;
    }

};

struct geom_handler_polygons {

    std::string output;

    void ring_begin() {
        output += "        RING(";
    }

    void ring_point(const vtzero::point& point) {
        output += std::to_string(point.x);
        output += ' ';
        output += std::to_string(point.y);
        output += ',';
    }

    void ring_end(bool is_outer) {
        if (output.empty()) {
            return;
        }
        if (output.back() == ',') {
            output.resize(output.size() - 1);
        }
        if (is_outer) {
            output += " OUTER";
        } else {
            output += " INNER";
        }
        output += ")\n";
        std::cout << output;
    }

};

struct print_value {

    template <typename T>
    void operator()(const T& value) const {
        std::cout << value;
    }

    void operator()(const vtzero::data_view& value) const {
        std::cout.write(value.data(), value.size());
    }

}; // struct print_value

void print_layer(vtzero::layer& layer, bool print_tables) {
    std::cout << "layer:\n"
              << "  name   : " << std::string{layer.name()} << '\n'
              << "  version: " << layer.version() << '\n'
              << "  extent : " << layer.extent() << '\n';

    if (print_tables) {
        std::cout << "  keys   :\n";
        int n = 0;
        for (const auto& key : layer.key_table()) {
            std::cout << "    " << n++ << ' ';
            std::cout.write(key.data(), key.size());
            std::cout << '\n';
        }
    }

    while (const auto feature = layer.get_next_feature()) {
        std::cout << "  feature:\n"
                  << "    id        : " << feature.id() << '\n'
                  << "    geomtype  : " << vtzero::geom_type_name(feature.type()) << '\n'
                  << "    geom      : ";
        switch (feature.type()) {
            case vtzero::GeomType::POINT:
                vtzero::decode_point_geometry(feature.geometry(), false, geom_handler_points{});
                break;
            case vtzero::GeomType::LINESTRING:
                vtzero::decode_linestring_geometry(feature.geometry(), false, geom_handler_linestrings{});
                break;
            case vtzero::GeomType::POLYGON:
                std::cout << '\n';
                vtzero::decode_polygon_geometry(feature.geometry(), false, geom_handler_polygons{});
                break;
            default:
                std::cout << "UNKNOWN GEOMETRY TYPE\n";
        }
        std::cout << "    tags:\n";
        for (auto tag : feature.tags(layer)) {
            std::cout << "      ";
            std::cout.write(tag.key().data(), tag.key().size());
            std::cout << '=';
            tag_value_visit(print_value{}, tag);
            std::cout << '\n';
        }
    }
}

void print_layer_overview(vtzero::layer& layer) {
    std::cout.write(layer.name().data(), layer.name().size());
    std::cout << ' ' << layer.get_feature_count() << '\n';
}

void print_help() {
    std::cout << "vtzero-show [OPTIONS] VECTOR-TILE [LAYER-NUM|LAYER-NAME]\n\n"
              << "Dump contents of vector tile.\n"
              << "\nOptions:\n"
              << "  -h, --help         This help message\n"
              << "  -l, --layers       Show layer overview\n";
}

int main(int argc, char* argv[]) {
    bool layer_overview = false;
    bool print_tables = false;

    static struct option long_options[] = {
        {"help",   no_argument, nullptr, 'h'},
        {"layers", no_argument, nullptr, 'l'},
        {"tables", no_argument, nullptr, 't'},
        {nullptr, 0, nullptr, 0}
    };

    while (true) {
        const int c = getopt_long(argc, argv, "hlt", long_options, nullptr);
        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help();
                std::exit(0);
            case 'l':
                layer_overview = true;
                break;
            case 't':
                print_tables = true;
                break;
            default:
                std::exit(1);
        }
    }

    const int remaining_args = argc - optind;
    if (remaining_args < 1 || remaining_args > 2) {
        std::cerr << "Usage: " << argv[0] << " [OPTIONS] VECTOR-TILE [LAYER-NUM|LAYER-NAME]\n";
        std::exit(1);
    }

    const auto data = read_file(argv[optind]);

    vtzero::vector_tile tile{data};

    if (remaining_args == 1) {
        while (auto layer = tile.get_next_layer()) {
            if (layer_overview) {
                print_layer_overview(layer);
            } else {
                print_layer(layer, print_tables);
            }
        }
    } else {
        auto layer = get_layer(tile, argv[optind + 1]);
        if (layer_overview) {
            print_layer_overview(layer);
        } else {
            print_layer(layer, print_tables);
        }
    }

}
