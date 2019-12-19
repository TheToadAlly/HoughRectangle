#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "io.hpp"
#include "string"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <array>
#include "stb_image.h"
#include "stb_image_write.h"

unsigned char uc_conv(int x) { return static_cast<unsigned char>(x); };

/////////////////////////////////////////////////////////////////////////////
// Raw buffer matcher
/////////////////////////////////////////////////////////////////////////////
class RawBufEq : public Catch::MatcherBase<unsigned char*> {
    unsigned char* m_arr1;
    int m_L;

   public:
    RawBufEq(unsigned char* arr1, int L) : m_arr1(arr1), m_L(L) {}

    // Matcher. If elements don't match, prints them
    bool match(unsigned char* const& arr2) const override {
        bool out = true;
        for (int i = 0; i < m_L; i++) {
            if (m_arr1[i] != arr2[i]) {
                std::cout << "elements " << i << ": arr1=" << (int)m_arr1[i] << ", arr2=" << (int)arr2[i] << std::endl;
                out = false;
            } else {
                std::cout << "elements " << i << ": arr1=" << (int)m_arr1[i] << ", arr2=" << (int)arr2[i] << std::endl;
            }
        }
        return out;
    }

    virtual std::string describe() const override {
        std::ostringstream ss;
        ss << "Arrays do not match";

        return ss.str();
    }
};

inline RawBufEq IsEqual(unsigned char* arr1, int L) { return RawBufEq(arr1, L); }

/////////////////////////////////////////////////////////////////////////////
// Tests
/////////////////////////////////////////////////////////////////////////////
TEST_CASE("Test Input-output functions for images") {
    std::string filename = "../unit_test/test_img.png";

    SECTION("Convert raw buffer to Eigen matrix") {
        // Convert some data to Eigen matrix
        std::unique_ptr<unsigned char[]> data(new unsigned char[12]);
        data[0] = (unsigned char)1;
        data[1] = (unsigned char)2;
        data[2] = (unsigned char)3;
        data[3] = (unsigned char)155;
        data[4] = (unsigned char)255;
        data[5] = (unsigned char)3;
        data[6] = (unsigned char)4;
        data[7] = (unsigned char)5;
        data[8] = (unsigned char)2;
        data[9] = (unsigned char)5;
        data[10] = (unsigned char)78;
        data[11] = (unsigned char)1;

        Eigen::MatrixXf gray;
        convert_RawBuff2Mat(data, gray, 4, 3);

        // Ground truth
        Eigen::MatrixXf ground_truth(3, 4);
        ground_truth << 1, 2, 3, 155, 255, 3, 4, 5, 2, 5, 78, 1;

        REQUIRE(ground_truth == gray);
    }

    SECTION("Converter from Eigen matrix to unsigned char raw buffer") {
        // Build input
        const int x = 4;
        const int y = 3;
        const int L = x * y;
        Eigen::Matrix<float, y, x, Eigen::RowMajor> gray;
        ;
        gray << 1.34, 3.908, 55.201, 255.0978, 0.097, 69.698, 25.09853, 94.8975, 30.309, 50.5, 38.0985, 50;

        int ground_truth_int[L] = {1, 4, 55, 255, 0, 70, 25, 95, 30, 51, 38, 50};

        // Populate GT. Replace this loop later
        std::unique_ptr<unsigned char[]> ground_truth(new unsigned char[L]);
        for (int i = 0; i < L; i++) {
            ground_truth[i] = static_cast<unsigned char>(ground_truth_int[i]);
        }

        // Function to test: converting matrix to raw buffer
        std::unique_ptr<unsigned char[]> gray_UC(new unsigned char[L]);
        convert_Mat2RawBuff(gray, gray_UC, 3 * 4);

        // Test Raw buffers equality
        REQUIRE_THAT(gray_UC.get(), IsEqual(ground_truth.get(), 12));
    }

    SECTION("Eigen matrix saver") {
        // Dummy matrix
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> gray(3, 6);
        gray << 2, 3, 1, 5, 6, 5, 5, 2, 6, 1, 9, 3, 3, 1, 5, 3, 2, 1;

        auto out = save_image(gray, "../unit_test/unit_test.png", 30, 6, 3);

        REQUIRE(out == 1);
    }

    SECTION("Image reader into Eigen matrix") {
        // Load image
        Eigen::MatrixXf img = read_image("../unit_test/test_img.png");

        // Ground truth
        Eigen::MatrixXf ground_truth(3, 4);
        ground_truth << 1, 2, 3, 155, 255, 3, 4, 5, 2, 5, 78, 1;

        REQUIRE(ground_truth == img);
    }

    SECTION("Save maximums to txt file") {
        std::vector<float> theta = {1, 2, 3};
        std::vector<float> rho = {4, 5, 6};
        std::string filename = "../unit_test/test_maximums.cpp";

        // save_maximum(filename,theta,rho);
    }

    SECTION("Convert normal line to cartesian coordinates") {
        std::array<float, 3> vert_line = convert_normal2cartesian(0, 20);
        std::array<float, 3> vert_line_gt = {1, 0, -20};

        std::array<float, 3> horiz_line = convert_normal2cartesian(90, 20);
        std::array<float, 3> horiz_line_gt = {0, 1, -20};

        std::cout << horiz_line[0] << " " << horiz_line[1] << " " << horiz_line[2] << std::endl;

        REQUIRE(vert_line == vert_line_gt);
        // REQUIRE( horiz_line == horiz_line_gt );

        // assert vert_line
    }

    SECTION("Convert rectangle to corners") {
        std::array<float, 3> rectangle_normal = {0, 20, 10};

        std::array<int, 8> rectangle = convert_normal_rect2_corners_rect(rectangle_normal, 0, 0);

        for (int corner : rectangle) std::cout << corner << std::endl;
    }
}
#endif
#endif

