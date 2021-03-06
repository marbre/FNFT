/*
* This file is part of FNFT.  
*                                                                  
* FNFT is free software; you can redistribute it and/or
* modify it under the terms of the version 2 of the GNU General
* Public License as published by the Free Software Foundation.
*
* FNFT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*                                                                      
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contributors:
* Sander Wahls (TU Delft) 2017-2018.
*/

/**
 * @file fnft__misc.h
 * @ingroup misc
 */

#ifndef FNFT__MISC_H
#define FNFT__MISC_H

#include "fnft.h"

/**
 * @brief Helper function for debugging. Prints an array in MATLAB style.
 * 
 * @ingroup misc
 * This function prints an array in MATLAB style.
 * @param[in] len Length of the array to be printed.
 * @param[in] buf Array to be printed.
 * @param[in] varname Name of the array being printed.
 */
void fnft__misc_print_buf(FNFT_INT len, FNFT_COMPLEX *buf, char *varname);

/**
 * @brief Relative l1 error between two vectors.
 * 
 * @ingroup misc
 * This function computes the relative l1 error between two vectors.\n
 * \f$err = \frac{\sum_{i=0}^{i=len-1} |vec\_numer[i]-vec\_exact[i]|}{\sum_{i=0}^{i=len-1} |vec\_exact[i]|}\f$.
 * @param[in] len Length of the vectors
 * @param[in] vec_numer Complex array of numerically computed result of length len.
 * @param[in] vec_exact Complex array of exact result of length len.
 * @return Returns the real valued relative error err.
 */
FNFT_REAL fnft__misc_rel_err(FNFT_INT len, FNFT_COMPLEX *vec_numer,
    FNFT_COMPLEX *vec_exact);

/**
 * @brief Hausdorff distance between two vectors.
 * 
 * @ingroup misc
 * This function computes the Hausdorff distance between two vectors vecA and vecB.
 * @param[in] lenA Length of vector vecA.
 * @param[in] vecA Complex vector of length lenA.
 * @param[in] lenB length of vector vecB.
 * @param[in] vecB Complex vector of length lenB.
 * @return Returns the real valued Hausdorff distance between the vectors vecA and vecB.
 */
FNFT_REAL fnft__misc_hausdorff_dist(const FNFT_UINT lenA,
    FNFT_COMPLEX const * const vecA, const FNFT_UINT lenB,
    FNFT_COMPLEX const * const vecB);

/**
 * @brief Hyperbolic secant.
 * 
 * @ingroup misc
 * This function returns the hyperbolic secant of a \link FNFT_COMPLEX \endlink.
 * @param[in] Z \link FNFT_COMPLEX \endlink argument.
 * @return hyperbolic secant of Z.
 */
FNFT_COMPLEX fnft__misc_sech(FNFT_COMPLEX Z);

/**
 * @brief Squared l2 norm. 
 * 
 * @ingroup misc
 * This function computes the quantity\n
 * \f$ val = \frac{b-a}{2N}.(|Z[0]|^2+|Z[N-1]|^2)+\sum_{i=1}^{i=N-2}\frac{b-a}{N}.|Z[i]|^2\f$.
 * @param[in] N Number of elements in the array.
 * @param[in] Z Complex valued array of length N.
 * @param[in] a Real number corresponding to first element of Z.
 * @param[in] b Real number corresponding to last element of Z.
 * @return Returns the quantity val. Returns NAN if N<2 or a>=b.
 */
FNFT_REAL fnft__misc_l2norm2(const FNFT_UINT N, FNFT_COMPLEX const * const Z,
    const FNFT_REAL a, const FNFT_REAL b);

/**
 * @brief Filters array by retaining elements inside a bounding box
 * 
 * @ingroup misc
 * This function filters the array vals. Only values that satisfy
 *
 *      bounding_box[0] <= real(val) <= bounding_box[1]
 * 
 * and
 *
 *      bounding_box[2] <= imag(val) <= bounding_box[3]
 *
 * are kept. 
 * @param[in,out] N It is the pointer to the number of values to be filtered. On exit *N is overwritten with
 * the number of values that have survived fitering. Their values will be
 * moved to the beginning of vals. 
 * @param[in,out] vals Complex valued array with elements to be filtered.
 * @param[in] rearrange_as_well Complex valued array. If the array rearrange_as_well is not NULL, 
 * then the values in there are rearranged together with the values in vals.
 * @param[in] bounding_box A real array of 4 elements. The elements determine the corners of the 
 * bounding box being used for filtering.
 * @return Returns SUCCESS or an error code.
 */
FNFT_INT fnft__misc_filter(FNFT_UINT * const N, FNFT_COMPLEX * const vals,
    FNFT_COMPLEX * const rearrange_as_well,
    FNFT_REAL const * const bounding_box);


/**
 * @brief Filter array based on specified tolerance.
 * 
 * @ingroup misc
 * This function removes all entries from the array vals with |Im(val)|>tol_im.
 * @param[in,out] N_ptr Pointer to number of values to be filtered. On exit *N_ptr is overwritten with
 * the number of values that have survived fitering. Their values will be
 * moved to the beginning of vals.
 * @param[in,out] vals Complex valued array with elements to be filtered.
 * @param[in] tol_im Real valued tolerance. 
 * @return Returns SUCCESS or an error code.
 */
FNFT_INT fnft__misc_filter_nonreal(FNFT_UINT *N_ptr, FNFT_COMPLEX * const vals,
    const FNFT_REAL tol_im);

/**
 * @brief Filters array by retaining elements outside a bounding box.
 * 
 * @ingroup misc
 * This function filters the array vals. Only values OUTSIDE the bounding box
 * are kept. 
 * @param[in,out] N_ptr It is the pointer to the number of values to be filtered. On exit *N_ptr is overwritten with
 * the number of values that have survived fitering. Their values will be
 * moved to the beginning of vals. 
 * @param[in,out] vals Complex valued array with elements to be filtered.
 * @param[in] rearrange_as_well Complex valued array. If the array rearrange_as_well is not NULL, 
 * then the values in there are rearranged together with the values in vals.
 * @param[in] bounding_box A real array of 4 elements. The elements determine the corners of the 
 * bounding box being used for filtering.
 * @return Returns SUCCESS or an error code.
 */
FNFT_INT fnft__misc_filter_inv(FNFT_UINT * const N_ptr, FNFT_COMPLEX * const vals,
    FNFT_COMPLEX * const rearrange_as_well,
    FNFT_REAL const * const bounding_box);

/**
 * @brief Merges elements in an array with distance lower than tol.
 * 
 * @ingroup misc
 * This function filters an array by merging elements if distance between the elements is less than tol.
 * @param[in,out] N_ptr It is the pointer to the number of elements to be filtered. On exit *N_ptr is overwritten with
 * the number of values that have survived fitering. Their values will be
 * moved to the beginning of vals. 
 * @param[in,out] vals Complex valued array with elements to be filtered.
 * @param[in] tol Real valued tolerance.
 * @return Returns SUCCESS or an error code.
 */
FNFT_INT fnft__misc_merge(FNFT_UINT *N_ptr, FNFT_COMPLEX * const vals,
    FNFT_REAL tol);

/**
 * @brief Computes a subsampled version of array q.
 * 
 * @ingroup misc
 * Computes a subsampled version of q. The length of q is D>=2. The routine
 * will allocate memory for the subsampled signal qsub and updates the
 * pointer *qsub_ptr such that it points to the newly allocated qsub. The
 * user is responsible to freeing the memory later. The new number of samples
 * Dsub>=2 and the subsampling factor D/Dsub are stored in *Dsub_ptr and
 * *subsampling_factor_ptr.
 * @param[in] q Complex valued array to be subsampled.
 * @param[in] D Number of samples in array q.
 * @param[out] qsub_ptr Pointer to the starting location of subsampled signal.
 * @param[out] Dsub_ptr Pointer to new number of samples.
 * @param[out] subsampling_factor_ptr Pointer to subsampling factor D/Dsub.
 * @return Returns SUCCESS or an error code.
 */
FNFT_INT fnft__misc_downsample(FNFT_COMPLEX const * const q, const FNFT_UINT D,
    FNFT_COMPLEX ** qsub_ptr, FNFT_UINT * const Dsub_ptr,
    FNFT_UINT * const subsampling_factor_ptr);

/**
 * @brief Sinc function for complex arguments.
 * 
 * @ingroup misc
 * Functions computes the Sinc function sin(x)/x for \link FNFT_COMPLEX \endlink argument.
 * @param[in] x \link FNFT_COMPLEX \endlink argument.
 * @returns Sinc(x).
 */
FNFT_COMPLEX fnft__misc_CSINC(FNFT_COMPLEX x);

#ifdef FNFT_ENABLE_SHORT_NAMES
#define misc_print_buf(...) fnft__misc_print_buf(__VA_ARGS__)
#define misc_rel_err(...) fnft__misc_rel_err(__VA_ARGS__)
#define misc_hausdorff_dist(...) fnft__misc_hausdorff_dist(__VA_ARGS__)
#define misc_sech(...) fnft__misc_sech(__VA_ARGS__)
#define misc_l2norm2(...) fnft__misc_l2norm2(__VA_ARGS__)
#define misc_filter(...) fnft__misc_filter(__VA_ARGS__)
#define misc_filter_inv(...) fnft__misc_filter_inv(__VA_ARGS__)
#define misc_filter_nonreal(...) fnft__misc_filter_nonreal(__VA_ARGS__)
#define misc_merge(...) fnft__misc_merge(__VA_ARGS__)
#define misc_downsample(...) fnft__misc_downsample(__VA_ARGS__)
#define misc_CSINC(...) fnft__misc_CSINC(__VA_ARGS__)
#endif

#endif
