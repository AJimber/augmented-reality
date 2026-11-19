#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include "common_code.hpp"

std::vector<cv::Point3f>
fsiv_generate_3d_calibration_points(const cv::Size &board_size,
                                    float square_size)
{
    std::vector<cv::Point3f> ret_v;

    for(int r = 1; r <= board_size.height; r++)
    {
        for(int c = 1; c <= board_size.width; c++)
        {
            ret_v.push_back(cv::Point3f(c * square_size, r * square_size, 0.0f));
        }
    }

    CV_Assert(ret_v.size() ==
              static_cast<size_t>(board_size.width * board_size.height));
    return ret_v;
}

bool fsiv_fast_find_chessboard_corners(const cv::Mat &img, const cv::Size &board_size,
                                       std::vector<cv::Point2f> &corner_points)
{
    CV_Assert(img.type() == CV_8UC3);
    bool was_found = false;

    was_found = cv::findChessboardCorners(img, board_size, corner_points, cv::CALIB_CB_FAST_CHECK);

    return was_found;
}

void fsiv_compute_camera_pose(const std::vector<cv::Point3f> &_3dpoints,
                              const std::vector<cv::Point2f> &_2dpoints,
                              const cv::Mat &camera_matrix,
                              const cv::Mat &dist_coeffs,
                              cv::Mat &rvec,
                              cv::Mat &tvec)
{
    CV_Assert(_3dpoints.size() >= 4 && _3dpoints.size() == _2dpoints.size());

    cv::solvePnP(_3dpoints, _2dpoints, camera_matrix, dist_coeffs, rvec, tvec);

    CV_Assert(rvec.rows == 3 && rvec.cols == 1 && rvec.type() == CV_64FC1);
    CV_Assert(tvec.rows == 3 && tvec.cols == 1 && tvec.type() == CV_64FC1);
}

void fsiv_draw_axes(cv::Mat &img,
                    const cv::Mat &camera_matrix, const cv::Mat &dist_coeffs,
                    const cv::Mat &rvec, const cv::Mat &tvec,
                    const float size, const int line_width)
{

    std::vector<cv::Point3f> _object_points {cv::Point3f(0, 0, 0), cv::Point3f(size, 0, 0),
            cv::Point3f(0, size, 0), cv::Point3f(0, 0, -size)};

    std::vector<cv::Point2f> image_points;
    cv::projectPoints(_object_points, rvec, tvec, camera_matrix, dist_coeffs, image_points);

    cv::line(img, image_points[0], image_points[1],
             cv::Scalar(255, 0, 0), line_width);

    cv::line(img, image_points[0], image_points[2],
             cv::Scalar(0, 255, 0), line_width);

    cv::line(img, image_points[0], image_points[3],
             cv::Scalar(0, 0, 255), line_width);

}

void fsiv_load_calibration_parameters(cv::FileStorage &fs,
                                      cv::Size &camera_size,
                                      float &error,
                                      cv::Mat &camera_matrix,
                                      cv::Mat &dist_coeffs,
                                      cv::Mat &rvec,
                                      cv::Mat &tvec)
{
    CV_Assert(fs.isOpened());

    fs["error"] >> error;
    fs["image-width"] >> camera_size.width;
    fs["image-height"] >> camera_size.height;
    fs["camera-matrix"] >> camera_matrix;
    fs["distortion-coefficients"] >> dist_coeffs;
    fs["rvec"] >> rvec;
    fs["tvec"] >> tvec;

    CV_Assert(fs.isOpened());
    CV_Assert(camera_matrix.type() == CV_64FC1 && camera_matrix.rows == 3 && camera_matrix.cols == 3);
    CV_Assert(dist_coeffs.type() == CV_64FC1 && dist_coeffs.rows == 1 && dist_coeffs.cols == 5);
    CV_Assert(rvec.type() == CV_64FC1 && rvec.rows == 3 && rvec.cols == 1);
    CV_Assert(tvec.type() == CV_64FC1 && tvec.rows == 3 && tvec.cols == 1);
    return;
}


void fsiv_draw_3d_model(cv::Mat &img, const cv::Mat &M,
                        const cv::Mat &dist_coeffs,
                        const cv::Mat &rvec, const cv::Mat &tvec,
                        const float size)
{
    CV_Assert(img.type() == CV_8UC3);

    static float desplazamiento = 0.0f;
    static float velocidad = 0.001f;

    desplazamiento += velocidad;

    if (desplazamiento >= size * 3.0f ||
        desplazamiento <= 0.0f)
        velocidad = -velocidad;

    float x = size + desplazamiento;
    float y = size;
    float h = size;

    // Base, parte superior y punta del tejado.
    std::vector<cv::Point3f> puntos3d = {
        {x,     y,     0},
        {x + h, y,     0},
        {x + h, y + h, 0},
        {x,     y + h, 0},

        {x,     y,     -h},
        {x + h, y,     -h},
        {x + h, y + h, -h},
        {x,     y + h, -h},

        {x + h/2, y + h/2, -h * 1.5f}
    };

    // Proyección de puntos 3D a 2D.
    std::vector<cv::Point2f> puntos2d;
    cv::projectPoints(puntos3d, rvec, tvec,
                      M, dist_coeffs, puntos2d);

    // Cada pareja representa una arista.
    const int aristas[][2] = {
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7},
        {4,8}, {5,8}, {6,8}, {7,8}
    };

    std::vector<std::vector<cv::Point>> lineas;

    for (const auto &arista : aristas)
    {
        lineas.push_back({
            cv::Point(puntos2d[arista[0]]),
            cv::Point(puntos2d[arista[1]])
        });
    }

    // Dibujar el modelo mediante sus aristas.
    cv::polylines(img, lineas, false,
                  cv::Scalar(0, 255, 255), 2,
                  cv::LINE_AA);
}


void fsiv_project_image(const cv::Mat &model, cv::Mat &scene,
                        const cv::Size &board_size,
                        const std::vector<cv::Point2f> &chess_board_corners)
{
    CV_Assert(!model.empty() && model.type() == CV_8UC3);
    CV_Assert(!scene.empty() && scene.type() == CV_8UC3);
    CV_Assert(static_cast<size_t>(board_size.area()) ==
              chess_board_corners.size());

    std::vector<cv::Point2f> board_corners {chess_board_corners[0], 
        chess_board_corners[board_size.width - 1],  
        chess_board_corners[board_size.height * board_size.width - 1],
        chess_board_corners[board_size.area() - board_size.width]};

    std::vector<cv::Point2f> img_corners{cv::Point2f(0, 0), cv::Point2f(model.cols - 1, 0),
        cv::Point2f(model.cols - 1, model.rows - 1), cv::Point2f(0, model.rows - 1)};
    
    cv::Mat M = cv::getPerspectiveTransform(img_corners, board_corners);
    cv::warpPerspective(model, scene, M, scene.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
}




