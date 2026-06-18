'''
Author: ZoYooJy zhouyujie_gdut@163.com
Date: 2026-06-16 19:19:25
LastEditors: ZoYooJy zhouyujie_gdut@163.com
LastEditTime: 2026-06-17 17:28:33
FilePath: /IMU_Calib_Util/scripts/analysis.py
'''
#!/usr/bin/env python3

"""
@file   analysis.py
@brief  Allan deviation analysis and IMU noise parameter fitting tool.
        Reads allan_variance.csv, extracts white noise, bias instability,
        and random walk parameters. Plots Allan deviation curves and
        outputs calibration parameters to the terminal.

Usage:
    python3 analysis.py --cfg analysis_cfg.yaml
"""

import argparse
import csv
import os

import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit


def line_func(x, m, b):
	"""Linear function y = m*x + b, used for fitting in log space"""
	return m * x + b

def get_intercept(x, y, m, b):
	"""
	In log-log space, fit data with a line of fixed slope m, return y at x=b.

	Args:
		x (np.array): x data (Allan deviation period)
		y (np.array): y data (Allan deviation value)
		m (float): Fixed slope of the fitting line
		b (float): x value at which to compute the intercept

	Returns:
		tuple: (intercept value yfit(b), fitting function yfit)
	"""
	logx = np.log(x)
	logy = np.log(y)
	coeffs, _ = curve_fit(line_func, logx, logy, bounds=([m, -np.inf], [m + 0.001, np.inf]))
	poly = np.poly1d(coeffs)
	yfit = lambda x: np.exp(poly(np.log(x)))

	return yfit(b), yfit


def generate_prediction(tau, q_quantization=0, q_white=0, q_bias_instability=0, q_walk=0, q_ramp=0):
	"""
	Generate Allan deviation prediction curve based on the five-term noise model.

	Args:
		tau (np.array): Sampling period
		q_quantization (float): Quantization noise
		q_white (float): White noise (velocity/angle random walk)
		q_bias_instability (float): Bias instability
		q_walk (float): Random walk (rate random walk)
		q_ramp (float): Rate ramp

	Returns:
		np.array: Predicted Allan deviation values
	"""
	n = len(tau)

	# Build coefficient matrix A for the five-term noise model
	A = np.empty((n, 5))
	A[:, 0] = 3 / tau**2         # Quantization noise term
	A[:, 1] = 1 / tau            # White noise term
	A[:, 2] = 2 * np.log(2) / np.pi  # Bias instability term
	A[:, 3] = tau / 3            # Random walk term
	A[:, 4] = tau**2 / 2         # Rate ramp term

	# Parameter vector
	params = np.array([q_quantization ** 2, q_white ** 2, q_bias_instability ** 2, q_walk ** 2, q_ramp ** 2])

	return np.sqrt(A.dot(params))


# ==================== Command-line argument parsing ====================
parser = argparse.ArgumentParser(description="Allan deviation analysis and IMU noise parameter fitting tool")
parser.add_argument('--cfg', metavar='STR', type=str, required=True,
                    help='Path to analysis_cfg.yaml (required)')
args = parser.parse_args()

# ==================== Load configuration file ====================
import yaml  # pip install pyyaml

with open(args.cfg, "r") as stream:
	cfg = yaml.safe_load(stream)

# Data directory: contains allan_variance.csv
data_dir = cfg.get("data_dir", ".")
# Figure save directory
fig_dir = cfg.get("fig_dir", ".")
# Calibration parameter save directory (reserved)
param_dir = cfg.get("param_dir", ".")
# Data skip interval
skip = cfg.get("skip", 1)

data_file = os.path.join(data_dir, "allan_variance.csv")

# Ensure output directories exist
os.makedirs(fig_dir, exist_ok=True)
os.makedirs(param_dir, exist_ok=True)

print(f"[INFO] Config file: {args.cfg}")
print(f"[INFO] Data file: {data_file}")
print(f"[INFO] Figure save directory: {fig_dir}")
print(f"[INFO] Parameter save directory: {param_dir}")
print(f"[INFO] Data skip interval: {skip}")

# ==================== Read Allan deviation data ====================
# CSV format (space-delimited): period, accX, accY, accZ, gyroX, gyroY, gyroZ
period = np.array([])
acceleration = np.empty((0, 3), float)
rotation_rate = np.empty((0, 3), float)

with open(data_file) as input_file:
	csv_reader = csv.reader(input_file, delimiter=' ')
	counter = 0

	for row in csv_reader:
		# Skip empty lines
		if len(row) == 0:
			continue
		# Filter out rows containing only empty strings
		row = [r for r in row if r.strip() != '']
		if len(row) < 7:
			continue

		counter = counter + 1
		if (counter % skip != 0):
			continue

		t = float(row[0])
		period = np.append(period, [t], axis=0)
		acceleration = np.append(acceleration, np.array([float(row[1]), float(row[2]), float(row[3])]).reshape(1, 3), axis=0)
		rotation_rate = np.append(rotation_rate, np.array([float(row[4]), float(row[5]), float(row[6])]).reshape(1, 3), axis=0)

print(f"[INFO] Loaded {len(period)} data points, period range: {period[0]:.2f}s ~ {period[-1]:.2f}s")


# Calculate intercepts and minimums
white_noise_break_point = np.where(period == 10)[0][0]

# ==================== Accel Data ====================

# White Noise Intercept with a line of the form y = -0.5 * x + 1.0
accel_wn_intercept_x, xfit_wn = get_intercept(period[0:white_noise_break_point], acceleration[0:white_noise_break_point,0], -0.5, 1.0)
accel_wn_intercept_y, yfit_wn = get_intercept(period[0:white_noise_break_point], acceleration[0:white_noise_break_point,1], -0.5, 1.0)
accel_wn_intercept_z, zfit_wn = get_intercept(period[0:white_noise_break_point], acceleration[0:white_noise_break_point,2], -0.5, 1.0)

# Random Walk Intercept with a line of the form y = 0.5 * x + 3.0
accel_rr_intercept_x, xfit_rr = get_intercept(period, acceleration[:,0], 0.5, 3.0)
accel_rr_intercept_y, yfit_rr = get_intercept(period, acceleration[:,1], 0.5, 3.0)
accel_rr_intercept_z, zfit_rr = get_intercept(period, acceleration[:,2], 0.5, 3.0)

accel_min_x = np.amin(acceleration[:,0])
accel_min_y = np.amin(acceleration[:,1])
accel_min_z = np.amin(acceleration[:,2])

accel_min_x_index = np.argmin(acceleration[:,0])
accel_min_y_index = np.argmin(acceleration[:,1])
accel_min_z_index = np.argmin(acceleration[:,2])

# Use worst value among all axes x,y,z
worst_accel_white_noise = np.amax([accel_wn_intercept_x, accel_wn_intercept_y, accel_wn_intercept_z])
worst_accel_random_walk = np.amax([accel_rr_intercept_x, accel_rr_intercept_y, accel_rr_intercept_z])

average_acc_white_noise = (accel_wn_intercept_x + accel_wn_intercept_y + accel_wn_intercept_z) / 3
average_acc_bias_instability = (accel_min_x + accel_min_y + accel_min_z) / 3
average_acc_random_walk = (accel_rr_intercept_x + accel_rr_intercept_y + accel_rr_intercept_z) / 3

print("ACCELEROMETER:")
print(f"X Accel Noise Density: {accel_wn_intercept_x: .5f} m/s^2/sqrt(Hz)")
print(f"Y Accel Noise Density: {accel_wn_intercept_y: .5f} m/s^2/sqrt(Hz)")
print(f"Z Accel Noise Density: {accel_wn_intercept_z: .5f} m/s^2/sqrt(Hz)")
print(f"Accel Noise Density Mean: {average_acc_white_noise: .5f} m/s^2/sqrt(Hz)")

print(f"X Accel Random Walk: {accel_rr_intercept_x: .5f} m/s^3/sqrt(Hz)")
print(f"Y Accel Random Walk: {accel_rr_intercept_y: .5f} m/s^3/sqrt(Hz)")
print(f"Z Accel Random Walk: {accel_rr_intercept_z: .5f} m/s^3/sqrt(Hz)")
print(f"Z Accel Random Walk Mean: {average_acc_random_walk: .5f} m/s^3/sqrt(Hz)")

print(f"X Accel Bias Instability: {accel_min_x: .5f} m/s^2")
print(f"Y Accel Bias Instability: {accel_min_y: .5f} m/s^2")
print(f"Z Accel Bias Instability: {accel_min_z: .5f} m/s^2")
print(f"Z Accel Bias Instability Mean: {average_acc_bias_instability: .5f} m/s^2")


### plot

dpi = 90
figsize = (16, 9)
fig1 = plt.figure(num="Acceleration", dpi=dpi, figsize=figsize)

plt.loglog(period, acceleration[:,0], "r--" , label='X')
plt.loglog(period, acceleration[:,1], "g--" , label='Y')
plt.loglog(period, acceleration[:,2], "b--" , label='Z')

plt.loglog(period, xfit_wn(period), "m-")
plt.loglog(period, yfit_wn(period), "m-")
plt.loglog(period, zfit_wn(period), "m-", label="White noise fit line")

plt.loglog(period, xfit_rr(period), "y-",)
plt.loglog(period, yfit_rr(period), "y-",)
plt.loglog(period, zfit_rr(period), "y-", label="Random Rate fit line")

plt.loglog(1.0, accel_wn_intercept_x, "ro", markersize=20)
plt.loglog(1.0, accel_wn_intercept_y, "go", markersize=20)
plt.loglog(1.0, accel_wn_intercept_z, "bo", markersize=20)

plt.loglog(3.0, accel_rr_intercept_x, "r*", markersize=20)
plt.loglog(3.0, accel_rr_intercept_y, "g*", markersize=20)
plt.loglog(3.0, accel_rr_intercept_z, "b*", markersize=20)

plt.loglog(period[accel_min_x_index], accel_min_x, "r^", markersize=20)
plt.loglog(period[accel_min_y_index], accel_min_y, "g^", markersize=20)
plt.loglog(period[accel_min_z_index], accel_min_z, "b^", markersize=20)

fitted_model = generate_prediction(period, q_white=average_acc_white_noise,
									q_bias_instability=average_acc_bias_instability, q_walk=average_acc_random_walk)
plt.loglog(period, fitted_model, "-k", label='fitted model')

plt.title("Accelerometer", fontsize=30)
plt.ylabel("Allan Deviation m/s^2", fontsize=30)
plt.legend(fontsize=25)
plt.grid(True)
plt.xlabel("Period (s)", fontsize=30)
plt.tight_layout()

plt.draw()
plt.pause(1)
w = plt.waitforbuttonpress(timeout=5)
plt.close()

fig1.savefig(os.path.join(fig_dir, 'acceleration.png'), dpi=600, bbox_inches = "tight")

# ==================== Gyro Data ====================

# Calculate intercepts and minimums for gyroscope
gyro_wn_intercept_x, xfit_wn = get_intercept(period[0:white_noise_break_point], rotation_rate[0:white_noise_break_point,0], -0.5, 1.0)
gyro_wn_intercept_y, yfit_wn = get_intercept(period[0:white_noise_break_point], rotation_rate[0:white_noise_break_point,1], -0.5, 1.0)
gyro_wn_intercept_z, zfit_wn = get_intercept(period[0:white_noise_break_point], rotation_rate[0:white_noise_break_point,2], -0.5, 1.0)

gyro_rr_intercept_x, xfit_rr = get_intercept(period, rotation_rate[:,0], 0.5, 3.0)
gyro_rr_intercept_y, yfit_rr = get_intercept(period, rotation_rate[:,1], 0.5, 3.0)
gyro_rr_intercept_z, zfit_rr = get_intercept(period, rotation_rate[:,2], 0.5, 3.0)

gyro_min_x = np.amin(rotation_rate[:,0])
gyro_min_y = np.amin(rotation_rate[:,1])
gyro_min_z = np.amin(rotation_rate[:,2])

gyro_min_x_index = np.argmin(rotation_rate[:,0])
gyro_min_y_index = np.argmin(rotation_rate[:,1])
gyro_min_z_index = np.argmin(rotation_rate[:,2])

# use worst value
worst_gyro_white_noise = np.amax([gyro_wn_intercept_x, gyro_wn_intercept_y, gyro_wn_intercept_z])
worst_gyro_random_walk = np.amax([gyro_rr_intercept_x, gyro_rr_intercept_y, gyro_rr_intercept_z])


average_gyro_white_noise = (gyro_wn_intercept_x + gyro_wn_intercept_y + gyro_wn_intercept_z) / 3
average_gyro_bias_instability = (gyro_min_x + gyro_min_y + gyro_min_z) / 3
average_gyro_random_walk = (gyro_rr_intercept_x + gyro_rr_intercept_y + gyro_rr_intercept_z) / 3

print("GYROSCOPE:")
print(f"X Gyro Noise Density: {gyro_wn_intercept_x: .5f} rad/s/sqrt(Hz)")
print(f"Y Gyro Noise Density: {gyro_wn_intercept_y: .5f} rad/s/sqrt(Hz)")
print(f"Z Gyro Noise Density: {gyro_wn_intercept_z: .5f} rad/s/sqrt(Hz)")
print(f"Z Gyro Noise Density Mean: {average_gyro_white_noise: .5f} rad/s/sqrt(Hz)")

print(f"X Gyro Random Walk: {gyro_rr_intercept_x: .5f} rad/s^2/sqrt(Hz)")
print(f"Y Gyro Random Walk: {gyro_rr_intercept_y: .5f} rad/s^2/sqrt(Hz)")
print(f"Z Gyro Random Walk: {gyro_rr_intercept_z: .5f} rad/s^2/sqrt(Hz)")
print(f"Z Gyro Random Walk Mean: {average_gyro_random_walk: .5f} rad/s^2/sqrt(Hz)")

print(f"X Gyro Bias Instability: {gyro_min_x: .5f} rad/s")
print(f"Y Gyro Bias Instability: {gyro_min_y: .5f} rad/s")
print(f"Z Gyro Bias Instability: {gyro_min_z: .5f} rad/s")
print(f"Z Gyro Bias Instability Mean: {average_gyro_bias_instability: .5f} rad/s")



### plot

fig2 = plt.figure(num="Gyro", dpi=dpi, figsize=figsize)

plt.loglog(period, rotation_rate[:,0], "r-" , label='X')
plt.loglog(period, rotation_rate[:,1], "g-" , label='Y')
plt.loglog(period, rotation_rate[:,2], "b-" , label='Z')

plt.loglog(period, xfit_wn(period), "m-")
plt.loglog(period, yfit_wn(period), "m-")
plt.loglog(period, zfit_wn(period), "m-", label="White noise fit line")

plt.loglog(period, xfit_rr(period), "y-")
plt.loglog(period, yfit_rr(period), "y-")
plt.loglog(period, zfit_rr(period), "y-", label="Random rate fit line")

plt.loglog(1.0, gyro_wn_intercept_x, "ro", markersize=20)
plt.loglog(1.0, gyro_wn_intercept_y, "go", markersize=20)
plt.loglog(1.0, gyro_wn_intercept_z, "bo", markersize=20)

plt.loglog(3.0, gyro_rr_intercept_x, "r*", markersize=20)
plt.loglog(3.0, gyro_rr_intercept_y, "g*", markersize=20)
plt.loglog(3.0, gyro_rr_intercept_z, "b*", markersize=20)

plt.loglog(period[gyro_min_x_index], gyro_min_x, "r^", markersize=20)
plt.loglog(period[gyro_min_y_index], gyro_min_y, "g^", markersize=20)
plt.loglog(period[gyro_min_z_index], gyro_min_z, "b^", markersize=20)

fitted_model = generate_prediction(period, q_white=average_gyro_white_noise,
									q_bias_instability=average_gyro_bias_instability, q_walk=average_gyro_random_walk)
plt.loglog(period, fitted_model, "-k", label='fitted model')

plt.title("Gyroscope", fontsize=30)
plt.ylabel("Allan Deviation rad/s", fontsize=30)
plt.legend(fontsize=25)
plt.grid(True)
plt.xlabel("Period (s)", fontsize=30)
plt.tight_layout()

plt.draw()
plt.pause(1)
w = plt.waitforbuttonpress(timeout=5)
plt.close()

fig2.savefig(os.path.join(fig_dir, 'gyro.png'), dpi=600, bbox_inches = "tight")