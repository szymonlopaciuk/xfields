// copyright ################################# //
// This file is part of the Xfields Package.   //
// Copyright (c) CERN, 2021.                   //
// ########################################### //

#ifndef XFIELDS_UNIFORM_BIN_SLICER_H
#define XFIELDS_UNIFORM_BIN_SLICER_H


/*gpufun*/
void UniformBinSlicer_slice(UniformBinSlicerData el,
                LocalParticle* part0,
                int64_t const use_bunch_index_array,
                int64_t const use_slice_index_array,
                /*gpuglmem*/ int64_t* i_slice_part,
                /*gpuglmem*/ int64_t* i_bunch_part){

    int64_t const num_slices = UniformBinSlicerData_get_num_slices(el);
    double const z_min = UniformBinSlicerData_get_z_min(el);
    double const dzeta = UniformBinSlicerData_get_dzeta(el);

    int64_t const num_bunches = UniformBinSlicerData_get_num_bunches(el);
    int64_t const i_bunch_0 = UniformBinSlicerData_get_i_bunch_0(el);
    double const bunch_spacing_zeta = UniformBinSlicerData_get_bunch_spacing_zeta(el);
    double* particles_per_slice = UniformBinSlicerData_getp1_particles_per_slice(el, 0);

    // Generated by:
    // coords = ['x', 'px', 'y', 'py', 'zeta', 'delta']
    //     for cc in coords:
    //         print(f"double* sum_{cc} = (UniformBinSlicerData_len_sum_{cc}(el) > 0) ?  UniformBinSlicerData_getp1_sum_{cc}(el, 0) : NULL ;")
    double* sum_x = (UniformBinSlicerData_len_sum_x(el) > 0) ?  UniformBinSlicerData_getp1_sum_x(el, 0) : NULL ;
    double* sum_px = (UniformBinSlicerData_len_sum_px(el) > 0) ?  UniformBinSlicerData_getp1_sum_px(el, 0) : NULL ;
    double* sum_y = (UniformBinSlicerData_len_sum_y(el) > 0) ?  UniformBinSlicerData_getp1_sum_y(el, 0) : NULL ;
    double* sum_py = (UniformBinSlicerData_len_sum_py(el) > 0) ?  UniformBinSlicerData_getp1_sum_py(el, 0) : NULL ;
    double* sum_zeta = (UniformBinSlicerData_len_sum_zeta(el) > 0) ?  UniformBinSlicerData_getp1_sum_zeta(el, 0) : NULL ;
    double* sum_delta = (UniformBinSlicerData_len_sum_delta(el) > 0) ?  UniformBinSlicerData_getp1_sum_delta(el, 0) : NULL ;

    // Generated by:
    // moms = ['x_x', 'x_px', 'x_y', 'x_py', 'x_zeta', 'x_delta',
    //         'px_px', 'px_y', 'px_py', 'px_zeta', 'px_delta',
    //         'y_y', 'y_py', 'y_zeta', 'y_delta',
    //         'py_py', 'py_zeta', 'py_delta',
    //         'zeta_zeta', 'zeta_delta',
    //         'delta_delta']
    // for mm in moms:
    //    print(f"double* sum_{mm} = UniformBinSlicerData_getp1_sum_{mm}(el, 0);")
    double* sum_x_x = UniformBinSlicerData_getp1_sum_x_x(el, 0);
    double* sum_x_px = UniformBinSlicerData_getp1_sum_x_px(el, 0);
    double* sum_x_y = UniformBinSlicerData_getp1_sum_x_y(el, 0);
    double* sum_x_py = UniformBinSlicerData_getp1_sum_x_py(el, 0);
    double* sum_x_zeta = UniformBinSlicerData_getp1_sum_x_zeta(el, 0);
    double* sum_x_delta = UniformBinSlicerData_getp1_sum_x_delta(el, 0);
    double* sum_px_px = UniformBinSlicerData_getp1_sum_px_px(el, 0);
    double* sum_px_y = UniformBinSlicerData_getp1_sum_px_y(el, 0);
    double* sum_px_py = UniformBinSlicerData_getp1_sum_px_py(el, 0);
    double* sum_px_zeta = UniformBinSlicerData_getp1_sum_px_zeta(el, 0);
    double* sum_px_delta = UniformBinSlicerData_getp1_sum_px_delta(el, 0);
    double* sum_y_y = UniformBinSlicerData_getp1_sum_y_y(el, 0);
    double* sum_y_py = UniformBinSlicerData_getp1_sum_y_py(el, 0);
    double* sum_y_zeta = UniformBinSlicerData_getp1_sum_y_zeta(el, 0);
    double* sum_y_delta = UniformBinSlicerData_getp1_sum_y_delta(el, 0);
    double* sum_py_py = UniformBinSlicerData_getp1_sum_py_py(el, 0);
    double* sum_py_zeta = UniformBinSlicerData_getp1_sum_py_zeta(el, 0);
    double* sum_py_delta = UniformBinSlicerData_getp1_sum_py_delta(el, 0);
    double* sum_zeta_zeta = UniformBinSlicerData_getp1_sum_zeta_zeta(el, 0);
    double* sum_zeta_delta = UniformBinSlicerData_getp1_sum_zeta_delta(el, 0);
    double* sum_delta_delta = UniformBinSlicerData_getp1_sum_delta_delta(el, 0);

    double const z_min_edge = z_min - 0.5 * dzeta;

    //start_per_particle_block (part0->part)
        double zeta = LocalParticle_get_zeta(part);
        double weight = LocalParticle_get_weight(part);
        const int64_t ipart = part->ipart;

        int64_t i_bunch_rel;
        double z_min_edge_bunch = z_min_edge;
        uint8_t can_be_assigned_to_slice = 0;

        if (num_bunches <= 0){
            i_bunch_rel = 0;
            can_be_assigned_to_slice = 1;
        }
        else{
            double i_bunch = floor((zeta - z_min_edge) / bunch_spacing_zeta);
            i_bunch_rel = i_bunch - i_bunch_0;
            if (i_bunch_rel >= 0 && i_bunch_rel < num_bunches){
                if (use_bunch_index_array){
                    i_bunch_part[ipart] = i_bunch;
                }
                z_min_edge_bunch = z_min_edge + i_bunch * bunch_spacing_zeta;
                can_be_assigned_to_slice = 1;
            } else {
                if (use_bunch_index_array){
                    i_bunch_part[ipart] = -1;
                }
                can_be_assigned_to_slice = 0;
            }
        }

        int64_t i_slice = floor((zeta - z_min_edge_bunch) / dzeta);
        if (can_be_assigned_to_slice && i_slice >= 0 && i_slice < num_slices){
            if (use_slice_index_array){
                i_slice_part[ipart] = i_slice;
            }

            atomicAdd(&particles_per_slice[i_slice + i_bunch_rel * num_slices], weight);

            // Generated by:
            // coords = ['x', 'px', 'y', 'py', 'zeta', 'delta']
            // for cc in coords:
            //     print(f"atomicAdd(&sum_{cc}[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_{cc}(part));")

            atomicAdd(&sum_x[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_x(part));
            atomicAdd(&sum_px[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_px(part));
            atomicAdd(&sum_y[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_y(part));
            atomicAdd(&sum_py[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_py(part));
            atomicAdd(&sum_zeta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_zeta(part));
            atomicAdd(&sum_delta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_delta(part));

            // Generated by:
            // moms = ['x_x', 'x_px', 'x_y', 'x_py', 'x_zeta', 'x_delta',
            //         'px_px', 'px_y', 'px_py', 'px_zeta', 'px_delta',
            //         'y_y', 'y_py', 'y_zeta', 'y_delta',
            //         'py_py', 'py_zeta', 'py_delta',
            //         'zeta_zeta', 'zeta_delta',
            //         'delta_delta']
            // for mm in moms:
            //     cc1, cc2 = mm.split('_')
            //     print(f"atomicAdd(&sum_{mm}[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_{cc1}(part) * LocalParticle_get_{cc2}(part));")

            atomicAdd(&sum_x_x[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_x(part) * LocalParticle_get_x(part));
            atomicAdd(&sum_x_px[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_x(part) * LocalParticle_get_px(part));
            atomicAdd(&sum_x_y[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_x(part) * LocalParticle_get_y(part));
            atomicAdd(&sum_x_py[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_x(part) * LocalParticle_get_py(part));
            atomicAdd(&sum_x_zeta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_x(part) * LocalParticle_get_zeta(part));
            atomicAdd(&sum_x_delta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_x(part) * LocalParticle_get_delta(part));
            atomicAdd(&sum_px_px[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_px(part) * LocalParticle_get_px(part));
            atomicAdd(&sum_px_y[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_px(part) * LocalParticle_get_y(part));
            atomicAdd(&sum_px_py[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_px(part) * LocalParticle_get_py(part));
            atomicAdd(&sum_px_zeta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_px(part) * LocalParticle_get_zeta(part));
            atomicAdd(&sum_px_delta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_px(part) * LocalParticle_get_delta(part));
            atomicAdd(&sum_y_y[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_y(part) * LocalParticle_get_y(part));
            atomicAdd(&sum_y_py[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_y(part) * LocalParticle_get_py(part));
            atomicAdd(&sum_y_zeta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_y(part) * LocalParticle_get_zeta(part));
            atomicAdd(&sum_y_delta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_y(part) * LocalParticle_get_delta(part));
            atomicAdd(&sum_py_py[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_py(part) * LocalParticle_get_py(part));
            atomicAdd(&sum_py_zeta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_py(part) * LocalParticle_get_zeta(part));
            atomicAdd(&sum_py_delta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_py(part) * LocalParticle_get_delta(part));
            atomicAdd(&sum_zeta_zeta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_zeta(part) * LocalParticle_get_zeta(part));
            atomicAdd(&sum_zeta_delta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_zeta(part) * LocalParticle_get_delta(part));
            atomicAdd(&sum_delta_delta[i_slice + i_bunch_rel * num_slices], weight * LocalParticle_get_delta(part) * LocalParticle_get_delta(part));

        } else {
            if (use_slice_index_array){
                i_slice_part[ipart] = -1;
            }
        }


    //end_per_particle_block

}

/*gpufun*/
void UniformBinSlicer_track_local_particle(UniformBinSlicerData el,
                LocalParticle* part0){

}

#endif