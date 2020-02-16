/** Interface file for two-dimensional plotting
 *
 *  \file plot/plot2d/Plot2d.h
 *  \author Philip Salvaggio, Ph.D. (psscis@rit.edu)
 *          Carl Salvaggio, Ph.D. (salvaggio@cis.rit.edu)
 *  \date 9 February 2020
 */

#pragma once

#include <string>
#include <vector>
#include <opencv2/core.hpp>

#include "imgs/third_party/gnuplot-iostream.h"

#include "imgs/plot/plot2d/Params.h"

namespace plot {

void set_common_parameters(std::string& gp_msg, const plot2d::Params& params);

/** Plot two-dimensional data
 *
 *  \param[in] number_of_data_series
 *      number of data series provided to include in the current plot
 *  \param[in] params
 *      plot parameters object of type plot2d::Params
 *  \param[in] callback
 *      callback function to deliver x and y data series
 */
template <class Callable>
void Plot2d(size_t number_of_data_series, const plot2d::Params& params,
            Callable callback) {
  std::string gp_msg;
  for (size_t series_idx = 0; series_idx < number_of_data_series;
       series_idx++) {
    gp_msg += "$data" + std::to_string(series_idx) + " << EOD\n";
    size_t point_idx = 0;
    double x;
    double y;
    while (callback(series_idx, point_idx++, x, y)) {
      gp_msg += std::to_string(x) + " " + std::to_string(y) + "\n";
    }
    gp_msg += "EOD\n";
  }

  set_common_parameters(gp_msg, params);

  for (size_t series_idx = 0; series_idx < number_of_data_series;
       series_idx++) {
    if (series_idx == 0) {
      gp_msg += "plot $data" + std::to_string(series_idx);
      switch (params.linestyle()) {
        case params.LINES:
          gp_msg += " with lines linetype -1";
          break;
        case params.POINTS:
          gp_msg += " with points pointtype 5";
          break;
        case params.LINES + params.POINTS:
          gp_msg += " with linespoints linestyle 5";
          break;
        default:
          gp_msg += " with lines linetype -1";
          break;
      }
      gp_msg += " linecolor '" + params.colors[series_idx];
    } else {
      gp_msg += "', $data" + std::to_string(series_idx);
      switch (params.linestyle()) {
        case params.LINES:
          gp_msg += " with lines linetype -1";
          break;
        case params.POINTS:
          gp_msg += " with points pointtype 5";
          break;
        case params.LINES + params.POINTS:
          gp_msg += " with linespoints linestyle 5";
          break;
        default:
          gp_msg += " with lines linetype -1";
          break;
      }
      gp_msg += " linecolor '" + params.colors[series_idx];
    }
  }
  gp_msg += "'\n";

  Gnuplot gp;
  gp << gp_msg << std::endl;
}

/** Convenience function for plotting two-dimensional vector data
 *
 *  \param[in] x independent variable vector of type T1
 *  \param[in] y dependent variable vector of type T2
 *  \param[in] p plot parameters object of type plot2d::Params
 */
template <class T1, class T2>
void Plot2d(const std::vector<T1>& x, const std::vector<T2>& y,
            const plot2d::Params& params) {
  Plot2d(1, params,
         [&](size_t, size_t point_idx, double& x_value, double& y_value) {
           if (point_idx < x.size()) {
             x_value = x[point_idx];
             y_value = y[point_idx];
             return true;
           }
           return false;
         });
}

/** Convenience function for plotting multiple two-dimensional vector data
 *
 *  \param[in] x independent variable vector of vectors of type T1
 *  \param[in] y dependent variable vector of vectors of type T2
 *  \param[in] p plot parameters object of type plot2d::Params
 */
template <class T1, class T2>
void Plot2d(const std::vector<std::vector<T1> >& x,
            const std::vector<std::vector<T2> >& y,
            const plot2d::Params& params) {
  Plot2d(x.size(), params,
         [&](size_t series_idx, size_t point_idx, double& x_value,
             double& y_value) {
           if (point_idx < x[series_idx].size()) {
             x_value = x[series_idx][point_idx];
             y_value = y[series_idx][point_idx];
             return true;
           }
           return false;
         });
}

/** Convenience function for plotting two-dimensional vector data stored
 *  in the row(s) of cv::Mat_
 *
 *  \param[in] x independent variable cv::Mat_ with 1 or multiple rows
 *  \param[in] y dependent variable cv::Mat_ with 1 or multiple rows
 *  \param[in] p plot parameters object of type plot2d::Params
 */
template <class T1, class T2>
void Plot2d(const cv::Mat_<T1> x, const cv::Mat_<T2> y,
            const plot2d::Params params) {
  Plot2d(x.rows, params,
         [&](size_t series_idx, size_t point_idx, double& x_value,
             double& y_value) {
           if (point_idx < (size_t)x.cols) {
             x_value = x(series_idx, point_idx);
             y_value = y(series_idx, point_idx);
             return true;
           }
           return false;
         });
}

}
