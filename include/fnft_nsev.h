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
 * @file fnft_nsev.h
 * @brief Fast nonlinear Fourier transform for the vanishing nonlinear
 *  Schroedinger equation.
 * @ingroup fnft
 */

#ifndef FNFT_NSEV_H
#define FNFT_NSEV_H

#include "fnft_nse_discretization_t.h"

/**
 * Enum that specifies how the bound states are filtered. Used in
 * \link fnft_nsev_opts_t \endlink.\n \n	
 * @ingroup data_types
 *  fnft_nsev_bsfilt_NONE: All detected roots of \f$ a(\lambda) \f$ are returned. \n \n
 *  fnft_nsev_bsfilt_BASIC: Only roots in the upper halfplane are returned and roots very close
 *  to each other are merged. \n \n
 *  fnft_nsev_bsfilt_FULL: Bound states in physically implausible regions are furthermore
 *  rejected. 
 */
typedef enum {
    fnft_nsev_bsfilt_NONE,
    fnft_nsev_bsfilt_BASIC,
    fnft_nsev_bsfilt_FULL
} fnft_nsev_bsfilt_t;

/**
 * Enum that specifies how the bound states are localized. Used in
 * \link fnft_nsev_opts_t \endlink. \n \n
 * @ingroup data_types
 *  fnft_nsev_bsloc_FAST_EIGENVALUE: A rooting finding routine due to Aurentz et al. (see
 *  https://arxiv.org/abs/1611.02435 and https://github.com/eiscor/eiscor)
 *  with \f$ O(D^2) \f$ complexity is used to detect the roots of
 *  \f$ a(\lambda) \f$. (Note: FNFT incorporates a development version of this
 *  routine as no release was available yet.) This method is relatively slow,
 *  but very reliable. \n \n
 *  fnft_nsev_bsloc_NEWTON: Newton's method is used to refine a given set of initial guesses.
 *  The discretization used for the the refinement is the one due to Boffetta and Osborne.
 *  The number of iterations is specified through the field \link fnft_nsev_opts_t::niter 
 *  \endlink.
 *  The array bound_states passed to \link fnft_nsev \endlink
 *  should contain the initial guesses and *K_ptr should specify the number of
 *  initial guesses. It is sufficient if bound_states and normconst_or_residues
 *  are of length *K_ptr in this case. This method can be very fast if good
 *  initial guesses for the bound states are available. The complexity is
 *  \f$ O(niter (*K\_ptr) D) \f$. \n \n
 *  fnft_nsev_bsloc_SUBSAMPLE_AND_REFINE: This method offers a good compromise between the other two. The
 *  method automatically finds initial guesses for the NEWTON method by first
 *  applying the FAST_EIGENVALUE method to a subsampled version of the signal.
 *  Second these initial guesses are refined using the NEWTON method. The
 *  subsampling factor is chosen such that the complexity is
 *  \f$ O(D \log^2 D + niter K D) \f$, where \f$ K \f$ is the number of bound
 *  states that survived the filtering operation of the initial call to the
 *  fnft_nsev_bsloc_FAST_EIGENVALUE method w.r.t. the subsampled signal.
 */
typedef enum {
    fnft_nsev_bsloc_FAST_EIGENVALUE,
    fnft_nsev_bsloc_NEWTON,
    fnft_nsev_bsloc_SUBSAMPLE_AND_REFINE
} fnft_nsev_bsloc_t;

/**
 * Enum that specifies the type of the discrete spectrum computed by the
 * routine. Used in \link fnft_nsev_opts_t \endlink.\n \n
 * @ingroup data_types
 *  fnft_nsev_dstype_NORMING_CONSTANTS: The array is filled with the norming constants
 *  \f$ b_k \f$. \n\n
 *  fnft_nsev_dstype_RESIDUES: The array is filled with the residues (aka spectral amplitudes)
 *  \f$ b_k\big/\frac{da(\lambda_k)}{d\lambda} \f$. \n \n
 *  fnft_nsev_dstype_BOTH: The array contains both, first the norming constants and then the
 *  residues. Note that the length of the array passed by the user has to be 2*(*K_ptr) in this case.
 */
typedef enum {
    fnft_nsev_dstype_NORMING_CONSTANTS,
    fnft_nsev_dstype_RESIDUES,
    fnft_nsev_dstype_BOTH
} fnft_nsev_dstype_t;

/**
 * Enum that specifies the type of the continuous spectrum computed by the
 * routine. Used in \link fnft_nsev_opts_t \endlink.\n \n 
 * @ingroup data_types
 *  fnft_nsev_cstype_REFLECTION_COEFFICIENT: The array is filled with the values of
 *  \f$ b(\xi)/a(\xi) \f$ on the grid specified in the description of
 *  \link fnft_nsev \endlink. \n\n
 *  fnft_nsev_cstype_AB: The array is filled with the values of \f$a(\xi)\f$ on the grid
 *  specified in the description of \link fnft_nsev \endlink, followed by
 *  the values of \f$ b(\xi) \f$ on the same grid. Note that the length of the
 *  array contspec passed by the user has to be 2(*M_ptr) in this case.\n\n
 */
typedef enum {
    fnft_nsev_cstype_REFLECTION_COEFFICIENT,
    fnft_nsev_cstype_AB,
    fnft_nsev_cstype_BOTH
} fnft_nsev_cstype_t;

/**
 * @struct fnft_nsev_opts_t
 * @brief Stores additional options for the routine \link fnft_nsev \endlink. 
 * @ingroup fnft
 * @ingroup data_types
 * 
 * Use the \link fnft_nsev_default_opts \endlink routine in order to generate
 * a new variable of this type with default options and modify as needed.
 *
 * @var fnft_nsev_opts_t::bound_state_filtering
 *  Controls how \link fnft_nsev \endlink decide whether a numerically found
 *  root of \f$ a(\lambda) \f$ is an actual bound state or not. \n 
 *  Should be of type \link fnft_nsev_bsfilt_t \endlink.
 *
 * @var fnft_nsev_opts_t::bound_state_localization
 *  Controls how \link fnft_nsev \endlink localizes bound states. \n 
 * Should be of type \link fnft_nsev_bsloc_t \endlink.  
 *
 * @var fnft_nsev_opts_t::niter
 *  Number of Newton iterations to be carried out when either the fnft_nsev_bsloc_NEWTON or
 *  the fnft_nsev_bsloc_SUBSAMPLE_AND_REFINE method is used.
 *
 * @var fnft_nsev_opts_t::discspec_type
 *  Controls how \link fnft_nsev \endlink fills the array
 *  normconsts_or_residues. \n 
 * Should be of type \link fnft_nsev_dstype_t \endlink.
 *
 * @var fnft_nsev_opts_t::contspec_type
 *  Controls how \link fnft_nsev \endlink fills the array
 *  contspec. \n 
 * Should be of type \link fnft_nsev_cstype_t \endlink.
 * 
 *
 * @var fnft_nsev_opts_t::normalization_flag
 *  Controls whether intermediate results during the fast forward scattering
 *  step are normalized. This takes a bit longer but sometimes increases the
 *  accuracy of the results. By default, normalization is enabled (i.e., the
 *  flag is one). To disable, set the flag to zero.\n\n
 *
 * @var fnft_nsev_opts_t::discretization
 *  Controls which discretization is applied to the continuous-time Zakharov-
 *  Shabat scattering problem. See \link fnft_nse_discretization_t \endlink.
 */
typedef struct {
    fnft_nsev_bsfilt_t bound_state_filtering;
	fnft_nsev_bsloc_t bound_state_localization;
    FNFT_UINT niter;
    fnft_nsev_dstype_t discspec_type;
    fnft_nsev_cstype_t contspec_type;
    FNFT_INT normalization_flag;
    fnft_nse_discretization_t discretization;
} fnft_nsev_opts_t;

/**
 * @brief Creates a new options variable for \link fnft_nsev \endlink with
 * default settings.
 *
 * @returns A \link fnft_nsev_opts_t \endlink object with the following options.\n
 *  bound_state_filtering = fnft_nsev_bsfilt_FULL\n
 *  bound_state_localization = fnft_nsev_bsloc_SUBSAMPLE_AND_REFINE\n
 *  niter = 10\n
 *  discspec_type = fnft_nsev_dstype_NORMING_CONSTANTS\n
 *  contspec_type = fnft_nsev_cstype_REFLECTION_COEFFICIENT\n
 *  normalization_flag = 1\n
 *  discretization = fnft_nse_discretization_2SPLIT4B\n
 *
  * @ingroup fnft
 */
fnft_nsev_opts_t fnft_nsev_default_opts();

/**
 * @brief Returns the maximum number of bound states that can be detected by
 * fnft_nsev.
 *
 * @param[in] D Number of samples that will be passed to
 * \link fnft_nsev \endlink. Should be
 *  larger than zero.
 * @param[in] opts Options that will be passed to fnft_nsev. If NULL is passed,
 *  the default options will be used.
 * @return Returns the maximum number of bound states or zero on error.
 *
 * @ingroup fnft
 */
FNFT_UINT fnft_nsev_max_K(const FNFT_UINT D,
    fnft_nsev_opts_t const * const opts);

/**
 * @brief Fast nonlinear Fourier transform for the nonlinear Schroedinger
 *  equation with vanishing boundary conditions.
 *
 * This routine computes the nonlinear Fourier transform for the nonlinear
 * Schroedinger equation \f[ iq_x + q_{tt} \pm 2q|q|^2=0, \quad  q=q(x,t), \f]
 * of Zakharov and Shabat (<a href="http://jetp.ac.ru/cgi-bin/e/index/e/34/1/p62?a=list">Soviet. Phys. JTEP 31(1), 1972</a>)
 * for initial conditions with vanishing boundaries
 * \f[ \lim_{t\to \pm \infty }q(x_0,t) = 0 \text{ sufficiently rapidly.} \f]
 * \n
 * The main references are:
 *      - Wahls and Poor,<a href="http://dx.doi.org/10.1109/ICASSP.2013.6638772">&quot;Introducing the fast nonlinear Fourier transform,&quot;</a> Proc. ICASSP 2013.
 *      - Wahls and Poor, <a href="http://dx.doi.org/10.1109/TIT.2015.2485944">&quot;Fast numerical nonlinear Fourier transforms,&quot;</a> IEEE Trans. Inform. Theor. 61(12), 2015.
 *      - Prins and Wahls, &quot;Higher order exponential splittings for the fast non-linear Fourier transform of the KdV equation,&quot; to appear in Proc. ICASSP 2018.
 *
 * The routine also utilizes ideas from the following papers:
 *      - Boffetta and Osborne, <a href="https://doi.org/10.1016/0021-9991(92)90370-E">&quot;Computation of the direct scattering transform for the nonlinear Schroedinger equation,&quot;</a> J. Comput. Phys. 102(2), 1992.
 *      - Aref, <a href="https://arxiv.org/abs/1605.06328">&quot;Control and Detection of Discrete Spectral Amplitudes in Nonlinear Fourier Spectrum,&quot;</a> Preprint, arXiv:1605.06328 [math.NA], May 2016.
 *      - Hari and Kschischang, <a href="https://doi.org/10.1109/JLT.2016.2577702">&quot;Bi-Directional Algorithm for Computing Discrete Spectral Amplitudes in the NFT,&quot; </a>J. Lightwave Technol. 34(15), 2016.
 *      - Aurentz et al., <a href="https://arxiv.org/abs/1611.02435">&quot;Roots of Polynomials: on twisted QR methods for companion matrices and pencils,&quot;</a> Preprint, arXiv:1611.02435 [math.NA]</a>, Dec. 2016.
 *
 * @param[in] D Number of samples
 * @param[in] q Array of length D, contains samples \f$ q(t_n)=q(x_0, t_n) \f$,
 *  where \f$ t_n = T[0] + n(T[1]-T[0])/(D-1) \f$ and \f$n=0,1,\dots,D-1\f$, of
 *  the to-be-transformed signal in ascending order
 *  (i.e., \f$ q(t_0), q(t_1), \dots, q(t_{D-1}) \f$)
 * @param[in] T Array of length 2, contains the position in time of the first and
 *  of the last sample. It should be T[0]<T[1].
 * @param[in] M Number of points at which the continuous spectrum (aka
 *  reflection coefficient) should be computed.
 * @param[out] contspec Array of length M in which the routine will store the
 *  desired samples \f$ r(\xi_m) \f$ of the continuous spectrum (aka
 *  reflection coefficient) in ascending order,
 *  where \f$ \xi_m = XI[0]+(XI[1]-XI[0])/(M-1) \f$ and \f$m=0,1,\dots,M-1\f$.
 *  Has to be preallocated by the user. If NULL is passed instead, the
 *  continuous spectrum will not be computed. By changing the options, it is
 *  also possible to compute the values of \f$ a(\xi) \f$ and \f$ b(\xi) \f$
 *  instead. In that case, twice the amount of memory has to be allocated.
 * @param[in] XI Array of length 2, contains the position of the first and the last
 *  sample of the continuous spectrum. It should be XI[0]<XI[1]. Can also be
 *  NULL if contspec==NULL.
 * @param[in,out] K_ptr Upon entry, *K_ptr should contain the length of the array
 *  bound_states. Upon return, *K_ptr contains the number of actually detected
 *  bound states. If the length of the array bound_states was not sufficient
 *  to store all of the detected bound states, a warning is printed and as many
 *  bound states as possible are returned instead.
 * @param[out] bound_states Array. Upon return, the routine has stored the detected
 *  bound states (aka eigenvalues) in the first *K_ptr entries of this array.
 *  If NULL is passed instead, the discrete spectrum will not be computed.
 *  Has to be preallocated by the user. The user can choose an arbitrary
 *  length. Typically, D is a good choice.
 * @param[out] normconsts_or_residues Array of the same length as bound_states. Upon
 *  return, the routine has stored the residues (aka spectral amplitudes)
 *  \f$\rho_k = b_k\big/ \frac{da(\lambda_k)}{d\lambda}\f$ in the
 *  first *K_ptr entries of this array. By passing a proper opts, it is also
 *  possible to store the norming constants (the '\f$ b_k \f$') or both. Has to
 *  be pre-allocated by the user. If NULL is passed instead, the residues
 *  will not be computed.
 * @param[in] kappa =+1 for the focusing nonlinear Schroedinger equation,
 *  =-1 for the defocusing one
 * @param[in] opts Pointer to a \link fnft_nsev_opts_t \endlink object. The object
 *  can be used to modify the behavior of the routine. Use
 *  the routine \link fnft_nsev_default_opts \endlink
 *  to generate such an object and modify as desired. It is also possible to
 *  pass NULL, in which case the routine will use the default options. The
 *  user is reponsible to freeing the object after the routine has returned.
 * @return \link FNFT_SUCCESS \endlink or one of the FNFT_EC_... error codes
 *  defined in \link fnft_errwarn.h \endlink.
 *
 * @ingroup fnft
 */
FNFT_INT fnft_nsev(const FNFT_UINT D, FNFT_COMPLEX * const q, 
    FNFT_REAL const * const T, const FNFT_UINT M, 
    FNFT_COMPLEX * const contspec, FNFT_REAL const * const XI, 
    FNFT_UINT * const K_ptr, FNFT_COMPLEX * const bound_states, 
    FNFT_COMPLEX * const normconsts_or_residues, const FNFT_INT kappa, 
    fnft_nsev_opts_t *opts);

#ifdef FNFT_ENABLE_SHORT_NAMES
#define nsev_bsfilt_NONE fnft_nsev_bsfilt_NONE
#define nsev_bsfilt_BASIC fnft_nsev_bsfilt_BASIC
#define nsev_bsfilt_FULL fnft_nsev_bsfilt_FULL
#define nsev_bsloc_FAST_EIGENVALUE fnft_nsev_bsloc_FAST_EIGENVALUE
#define nsev_bsloc_NEWTON fnft_nsev_bsloc_NEWTON
#define nsev_bsloc_SUBSAMPLE_AND_REFINE fnft_nsev_bsloc_SUBSAMPLE_AND_REFINE
#define nsev_dstype_NORMING_CONSTANTS fnft_nsev_dstype_NORMING_CONSTANTS
#define nsev_dstype_RESIDUES fnft_nsev_dstype_RESIDUES
#define nsev_dstype_BOTH fnft_nsev_dstype_BOTH
#define nsev_cstype_REFLECTION_COEFFICIENT fnft_nsev_cstype_REFLECTION_COEFFICIENT
#define nsev_cstype_AB fnft_nsev_cstype_AB
#define nsev_cstype_BOTH fnft_nsev_cstype_BOTH
#endif

#endif
