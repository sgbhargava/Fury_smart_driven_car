/*
 * gps_filter.hpp
 *
 *  Created on: Dec 13, 2015
 *      Author: Abhi
 */

#ifndef L5_APPLICATION_GPS_FILTER_HPP_
#define L5_APPLICATION_GPS_FILTER_HPP_

#include <stdio.h>
#include "filter_matrix.hpp"

/* Refer to http://en.wikipedia.org/wiki/Kalman_filter for
   mathematical details. The naming scheme is that variables get names
   that make sense, and are commented with their analog in
   the Wikipedia mathematical notation.
   This Kalman filter implementation does not support controlled
   input.
   (Like knowing which way the steering wheel in a car is turned and
   using that to inform the model.)
   Vectors are handled as n-by-1 matrices.
   TODO: comment on the dimension of the matrices */
typedef struct {
  /* k */
  int timestep;

  /* These parameters define the size of the matrices. */
  int state_dimension, observation_dimension;

  /* This group of matrices must be specified by the user. */
  /* F_k */
  Matrix state_transition;
  /* H_k */
  Matrix observation_model;
  /* Q_k */
  Matrix process_noise_covariance;
  /* R_k */
  Matrix observation_noise_covariance;

  /* The observation is modified by the user before every time step. */
  /* z_k */
  Matrix observation;

  /* This group of matrices are updated every time step by the filter. */
  /* x-hat_k|k-1 */
  Matrix predicted_state;
  /* P_k|k-1 */
  Matrix predicted_estimate_covariance;
  /* y-tilde_k */
  Matrix innovation;
  /* S_k */
  Matrix innovation_covariance;
  /* S_k^-1 */
  Matrix inverse_innovation_covariance;
  /* K_k */
  Matrix optimal_gain;
  /* x-hat_k|k */
  Matrix state_estimate;
  /* P_k|k */
  Matrix estimate_covariance;

  /* This group is used for meaningless intermediate calculations */
  Matrix vertical_scratch;
  Matrix small_square_scratch;
  Matrix big_square_scratch;

} KalmanFilter;

KalmanFilter alloc_filter(int state_dimension,
              int observation_dimension);

void free_filter(KalmanFilter f);

/* Runs one timestep of prediction + estimation.

   Before each time step of running this, set f.observation to be the
   next time step's observation.

   Before the first step, define the model by setting:
   f.state_transition
   f.observation_model
   f.process_noise_covariance
   f.observation_noise_covariance

   It is also advisable to initialize with reasonable guesses for
   f.state_estimate
   f.estimate_covariance
*/
void update(KalmanFilter f);

/* Just the prediction phase of update. */
void predict(KalmanFilter f);
/* Just the estimation phase of update. */
void estimate(KalmanFilter f);

/* Create a GPS filter that only tracks two dimensions of position and
   velocity.
   The inherent assumption is that changes in velocity are randomly
   distributed around 0.
   Noise is a parameter you can use to alter the expected noise.
   1.0 is the original, and the higher it is, the more a path will be
   "smoothed".
   Free with free_filter after using. */
KalmanFilter alloc_filter_velocity2d(double noise);

/* Set the seconds per timestep in the velocity2d model. */
void set_seconds_per_timestep(KalmanFilter f,
                  double seconds_per_timestep);

/* Update the velocity2d model with new gps data. */
void update_velocity2d(KalmanFilter f, double lat, double lon,
               double seconds_since_last_update);

/* Read a lat,long pair from a file.
   Format is lat,long<ignored>
   Return whether there was a lat,long to be read */
int read_lat_long(FILE* file, double* lat, double* lon);

/* Extract a lat long from a velocity2d Kalman filter. */
void get_lat_long(KalmanFilter f, double* lat, double* lon);

/* Extract velocity with lat-long-per-second units from a velocity2d
   Kalman filter. */
void get_velocity(KalmanFilter f, double* delta_lat, double* delta_lon);

/* Convert a lat, long, delta lat, and delta long into mph.*/
double calculate_metersPerSecond(double lat, double lon,
             double delta_lat, double delta_lon);

/* Extract speed in miles per hour from a velocity2d Kalman filter. */
double get_metersPerSecond(KalmanFilter f);



#endif /* L5_APPLICATION_GPS_FILTER_HPP_ */
