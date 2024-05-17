//
// Created by waxz on 5/14/24.
//
#include "pointcloud_process.h"

#include "math/transform/eigen_transform.h"

#include "common/clock_time.h"

#include "math/transform/transform3d.h"

#include "math/geometry/normal_estimation_3d.h"

#include <iostream>
typedef Eigen::Matrix<float,3,Eigen::Dynamic,Eigen::ColMajor> Matrix3xf;
typedef Eigen::Map<Matrix3xf> Matrix3xfMap;
typedef Eigen::Map<const Matrix3xf> Matrix3xfMapConst;   // a read-only map

int se3_inverse(f32_t tx, f32_t ty, f32_t tz,  f32_t roll, f32_t pitch, f32_t yaw,f32_t* tx_inv, f32_t* ty_inv, f32_t* tz_inv,  f32_t* roll_inv, f32_t* pitch_inv, f32_t* yaw_inv){
    auto transform = transform::createSe3(tx,ty,tz,roll,pitch,yaw);

    transform = transform.inverse();
    float ix, iy, iz, iroll, ipitch, iyaw;
    transform::extractSe3(transform, ix, iy, iz, iroll, ipitch, iyaw);
    *tx_inv = ix;
    *ty_inv = iy;
    *tz_inv = iz;
    *roll_inv = iroll;
    *pitch_inv = ipitch;
    *yaw_inv = iyaw;
    return 0;
}

int pointcloud_norm(float* src_buffer, u64_t point_num, u64_t* index_buffer, u64_t index_num, f32_t vx, f32_t vy, f32_t vz, f32_t* cx, f32_t* cy, f32_t* cz, f32_t* nx, f32_t* ny, f32_t* nz, f32_t* nd){

    if (index_num < 3 || point_num < 3){
        return -1;
    }
    perception::NormalEst3d normalEst3D;
    normalEst3D.setViewerPoint(vx, vy, vz);

    f32_t sum_x = 0.0;
    f32_t sum_y = 0.0;
    f32_t sum_z = 0.0;
    for( u64_t i = 0 ; i < index_num; i++ ){
        u64_t j = index_buffer[i]*3;
        sum_x += src_buffer[j] ;
        sum_y += src_buffer[j + 1] ;
        sum_z += src_buffer[j + 2] ;
    }
    f32_t ccx = 0.0, ccy = 0.0, ccz = 0.0;
    *cx = ccx = sum_x/static_cast<float>(index_num);
    *cy = ccy = sum_y/static_cast<float>(index_num);
    *cz = ccz = sum_z/static_cast<float>(index_num);

    normalEst3D.addCenter(ccx, ccy,ccz );
    for( u64_t i = 0 ; i < index_num; i++ ){
        u64_t j = index_buffer[i]*3;
        normalEst3D.addPoint(src_buffer[j], src_buffer[j+1], src_buffer[j + 2]);
    }
    f32_t cnx, cny, cnz, cnd;
    normalEst3D.compute(cnx, cny, cnz, cnd);
    *nx = cnx;
    *ny = cny;
    *nz = cnz;
    *nd = cnd;
    return 0;
}

int pointcloud_clip(float* src_buffer, u64_t height, u64_t width,float* dst_buffer ,u64_t filter_height_min, u64_t filter_height_max, u64_t filter_width_min, u64_t filter_width_max  ){

    if(filter_height_min >=filter_height_max
    || filter_height_max >  height

    || filter_width_min >= filter_width_max
    || filter_width_max > width
    ){
        return -1;
    }

    size_t width_offset = width*3;
//
    size_t clip_width = filter_width_max - filter_width_min;
    size_t clip_width_offset = clip_width*3;

    size_t clip_height = filter_height_max - filter_height_min;
    size_t clip_width_byte = clip_width_offset* sizeof(float);
    size_t filter_width_min_offset= filter_width_min*3;


    for(size_t i = 0 ; i < clip_height; i++){
        float* src = src_buffer + (filter_height_min + i )*width_offset + filter_width_min_offset;
        float* dst = dst_buffer + i*clip_width_offset;
        memcpy(dst, src, clip_width_byte);
    }


    return 0;
}


int pointcloud_transform(float* src_buffer, u64_t point_num, float* dst_buffer , f32_t tx, f32_t ty, f32_t tz,  f32_t roll, f32_t pitch, f32_t yaw){
    auto transform = transform::createSe3(tx,ty,tz,roll,pitch,yaw);
//    std::cout << "transform:\n" << transform.matrix() << "\n";
    auto src_mat = Matrix3xfMapConst(src_buffer, 3, point_num);
//    std::cout << "src_mat:\n" << src_mat << "\n";
    auto dst_mat = Matrix3xfMap(dst_buffer, 3, point_num);
    dst_mat = transform * src_mat;
//    std::cout << "dst_mat:\n" << dst_mat << "\n";

    return 0;
}