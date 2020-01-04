#include <iostream>
#include "cxxopts.hpp"
#include "Eigen/Dense"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "io.hpp"
#include "process_image.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include "config.hpp"
#include <fstream>
#include <vector>

using namespace Eigen;

int main(int argc, char * argv[]){
    ////////////////////////////////////////////////////////////////////////
    // Parse arguments
    ////////////////////////////////////////////////////////////////////////
    cxxopts::Options options("Runs windowed hough transfrom on an edge image");
    options.add_options()("i,image_path","Path to input image",cxxopts::value<std::string>())
        ("o,out_img_path","Path to save image (png)",cxxopts::value<std::string>());
    auto parsing_results = options.parse(argc,argv);

    std::string input_path = parsing_results["image_path"].as<std::string>();
    std::string output_path = parsing_results["out_img_path"].as<std::string>();

    // Parse config file
    Config config;
    std::ifstream is("../src/configs.json");
    cereal::JSONInputArchive archive(is);
    archive(config);

    ////////////////////////////////////////////////////////////////////////
    // Load image and prepare matrix
    ////////////////////////////////////////////////////////////////////////
    Matrix<float, Dynamic, Dynamic, RowMajor> gray = eigen_io::read_image(input_path.c_str());

    ////////////////////////////////////////////////////////////////////////
    // Process image
    ////////////////////////////////////////////////////////////////////////
    int thetaBins =256;
    int thetaMin = -90;
    int thetaMax = 90;
    int rhoBins =256;

    // Compute Hough transform
    HoughRectangle ht(gray,thetaBins,rhoBins,thetaMin,thetaMax);
    HoughRectangle::fMat wht = ht.hough_transform(gray);

    // Detect peaks
    std::vector<std::array<int, 2>> indexes = find_local_maximum(wht, 25);
    std::vector<float> rho_maxs, theta_maxs;
    std::tie(rho_maxs, theta_maxs) = ht.index_rho_theta(indexes);

    // Find pairs
    std::vector<std::array<float,4>> pairs = ht.find_pairs(rho_maxs,theta_maxs,1,2,1);
    
    // Save pairs
    eigen_io::save_pairs(output_path, pairs);
}