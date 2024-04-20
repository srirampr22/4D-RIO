// SPDX-License-Identifier: BSD-2-Clause

#include <rrio/registrations.hpp>

#include <iostream>

#include <pcl/registration/ndt.h>
#include <pcl/registration/icp.h>
#include <pcl/registration/gicp.h>

#include <pclomp/ndt_omp.h>
#include <pclomp/gicp_omp.h>
#include <fast_gicp/gicp/fast_gicp.hpp>
#include <fast_gicp/gicp/fast_vgicp.hpp>
#include <fast_gicp/gicp/fast_apdgicp.hpp>

#ifdef USE_VGICP_CUDA
#include <fast_gicp/gicp/fast_vgicp_cuda.hpp>
#endif

namespace rrio {

pcl::Registration<pcl::PointXYZI, pcl::PointXYZI>::Ptr select_registration_method(ros::NodeHandle& pnh) {
  using PointT = pcl::PointXYZI;

  // select a registration method (ICP, GICP, NDT)
  std::string registration_method = pnh.param<std::string>("registration_method", "FAST_APDGICP");
  if(registration_method == "FAST_GICP") {
    std::cout << "registration: FAST_GICP" << std::endl;
    fast_gicp::FastGICP<PointT, PointT>::Ptr gicp(new fast_gicp::FastGICP<PointT, PointT>());
    gicp->setNumThreads(pnh.param<int>("reg_num_threads", 0));
    gicp->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.01));
    gicp->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
    gicp->setMaxCorrespondenceDistance(pnh.param<double>("reg_max_correspondence_distance", 2.0));
    gicp->setCorrespondenceRandomness(pnh.param<int>("reg_correspondence_randomness", 20));
    return gicp;
  }
  if(registration_method == "FAST_APDGICP") {
    std::cout << "registration: FAST_APDGICP" << std::endl;
    fast_gicp::FastAPDGICP<PointT, PointT>::Ptr apdgicp(new fast_gicp::FastAPDGICP<PointT, PointT>());
    apdgicp->setNumThreads(pnh.param<int>("reg_num_threads", 0));
    apdgicp->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.1)); // 0.01
    apdgicp->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
    apdgicp->setMaxCorrespondenceDistance(pnh.param<double>("reg_max_correspondence_distance", 2.5));
    apdgicp->setCorrespondenceRandomness(pnh.param<int>("reg_correspondence_randomness", 20));
    apdgicp->setDistVar(pnh.param<double>("dist_var", 0.86));
    apdgicp->setAzimuthVar(pnh.param<double>("azimuth_var", 0.5));
    apdgicp->setElevationVar(pnh.param<double>("elevation_var", 1.0));
    return apdgicp;
  }
#ifdef USE_VGICP_CUDA
  else if(registration_method == "FAST_VGICP_CUDA") {
    std::cout << "registration: FAST_VGICP_CUDA" << std::endl;
    fast_gicp::FastVGICPCuda<PointT, PointT>::Ptr vgicp(new fast_gicp::FastVGICPCuda<PointT, PointT>());
    vgicp->setResolution(pnh.param<double>("reg_resolution", 1.0));
    vgicp->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.01));
    vgicp->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
    vgicp->setCorrespondenceRandomness(pnh.param<int>("reg_correspondence_randomness", 20));
    return vgicp;
  }
#endif
  else if(registration_method == "FAST_VGICP") {
    std::cout << "registration: FAST_VGICP" << std::endl;
    fast_gicp::FastVGICP<PointT, PointT>::Ptr vgicp(new fast_gicp::FastVGICP<PointT, PointT>());
    vgicp->setNumThreads(pnh.param<int>("reg_num_threads", 0));
    vgicp->setResolution(pnh.param<double>("reg_resolution", 1.0));
    vgicp->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.01));
    vgicp->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
    vgicp->setCorrespondenceRandomness(pnh.param<int>("reg_correspondence_randomness", 20));
    return vgicp;
  } else if(registration_method == "ICP") {
    std::cout << "registration: ICP" << std::endl;
    pcl::IterativeClosestPoint<PointT, PointT>::Ptr icp(new pcl::IterativeClosestPoint<PointT, PointT>());
    icp->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.01));
    icp->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
    icp->setMaxCorrespondenceDistance(pnh.param<double>("reg_max_correspondence_distance", 2.5));
    icp->setUseReciprocalCorrespondences(pnh.param<bool>("reg_use_reciprocal_correspondences", false));
    return icp;
  } else if(registration_method.find("GICP") != std::string::npos) {
    if(registration_method.find("OMP") == std::string::npos) {
      std::cout << "registration: GICP" << std::endl;
      pcl::GeneralizedIterativeClosestPoint<PointT, PointT>::Ptr gicp(new pcl::GeneralizedIterativeClosestPoint<PointT, PointT>());
      gicp->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.01));
      gicp->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
      gicp->setUseReciprocalCorrespondences(pnh.param<bool>("reg_use_reciprocal_correspondences", false));
      gicp->setMaxCorrespondenceDistance(pnh.param<double>("reg_max_correspondence_distance", 2.5));
      gicp->setCorrespondenceRandomness(pnh.param<int>("reg_correspondence_randomness", 20));
      gicp->setMaximumOptimizerIterations(pnh.param<int>("reg_max_optimizer_iterations", 20));
      return gicp;
    } else {
      std::cout << "registration: GICP_OMP" << std::endl;
      pclomp::GeneralizedIterativeClosestPoint<PointT, PointT>::Ptr gicp(new pclomp::GeneralizedIterativeClosestPoint<PointT, PointT>());
      gicp->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.01));
      gicp->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
      gicp->setUseReciprocalCorrespondences(pnh.param<bool>("reg_use_reciprocal_correspondences", false));
      gicp->setMaxCorrespondenceDistance(pnh.param<double>("reg_max_correspondence_distance", 2.5));
      gicp->setCorrespondenceRandomness(pnh.param<int>("reg_correspondence_randomness", 20));
      gicp->setMaximumOptimizerIterations(pnh.param<int>("reg_max_optimizer_iterations", 20));
      return gicp;
    }
  } else {
    if(registration_method.find("NDT") == std::string::npos) {
      std::cerr << "warning: unknown registration type(" << registration_method << ")" << std::endl;
      std::cerr << "       : use NDT" << std::endl;
    }

    double ndt_resolution = pnh.param<double>("reg_resolution", 0.5);
    if(registration_method.find("OMP") == std::string::npos) {
      std::cout << "registration: NDT " << ndt_resolution << std::endl;
      pcl::NormalDistributionsTransform<PointT, PointT>::Ptr ndt(new pcl::NormalDistributionsTransform<PointT, PointT>());
      ndt->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.01));
      ndt->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
      ndt->setResolution(ndt_resolution);
      return ndt;
    } else {
      int num_threads = pnh.param<int>("reg_num_threads", 0);
      std::string nn_search_method = pnh.param<std::string>("reg_nn_search_method", "KDTREE"); // DIRECT1, DIRECT7
      std::cout << "registration: NDT_OMP " << nn_search_method << " " << ndt_resolution << " (" << num_threads << " threads)" << std::endl;
      pclomp::NormalDistributionsTransform<PointT, PointT>::Ptr ndt(new pclomp::NormalDistributionsTransform<PointT, PointT>());
      if(num_threads > 0) {
        ndt->setNumThreads(num_threads);
      }
      ndt->setTransformationEpsilon(pnh.param<double>("reg_transformation_epsilon", 0.01));
      ndt->setMaximumIterations(pnh.param<int>("reg_maximum_iterations", 64));
      ndt->setResolution(ndt_resolution);
      if(nn_search_method == "KDTREE") {
        ndt->setNeighborhoodSearchMethod(pclomp::KDTREE);
      } else if(nn_search_method == "DIRECT1") {
        ndt->setNeighborhoodSearchMethod(pclomp::DIRECT1);
      } else {
        ndt->setNeighborhoodSearchMethod(pclomp::DIRECT7);
      }
      return ndt;
    }
  }

  return nullptr;
}

}  // namespace rrio
