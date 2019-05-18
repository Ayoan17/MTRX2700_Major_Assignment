#include <stdio.h>
#include <math.h>

#define PIE 3.141582654

// Processes the accelerometer raw data to convert into scale of +/- 2g and also averages based on sample number
void getConvertedAccel(int sample_number, float* accelX, float* accelY, float* accelZ){
	float totalX = 0;
	float totalY = 0;
	float totalZ = 0;
	int i_acell = 0;
	
	//Save the data from the accelerometer
	for(i_acell; i_acell < sample_number; i_acell++){
		adxl345_getrawdata( &axraw, &ayraw, &azraw);
		totalX += axraw[0];
		totalY += (ayraw[0] - 6);
		totalZ += (azraw[0] - 56);
		delay1(13);       // delay1 function already exists in the code which pulls out raw data from sensors
	}
	
	//Get average of the values
	totalX = totalX/sample_number;
	totalY = totalY/sample_number;
	totalZ = totalZ/sample_number;
	
	/* Use FS ±2 g as sensitivity level. As the range is -2 to +2, this would be a total of 4g.  
	Or 4,000 Milli-Gs. The output is 16 bits. 16 bits equals 65,535.   
	This means we can get 65,535 different readings for the range  between -2 and +2. (or -2,000 MilliGs and +2,000 MilliGs)
	4,000 MilliGs / 65,535 = 0.061 
	Therefore, multiply raw data with 0.061 to get answer in terms of 4g and substract by 2g to scale it to +/- 2g. */
	totalX = (totalX*0.061)/1000 - 2 ;
	totalY = (totalY*0.061)/1000 - 2 ;
	totalZ = (totalZ*0.061)/1000 - 2 ;
	
	*accelX = totalX ;
	*accelY = totalY ;
	*accelZ = totalZ ;
}


// Processes the gyro raw data to convert into scale of +/- 2000 degrees per second and also averages based on sample number
void getConvertedGyro(int sample_number, float* gyroX, float* gyroY, float* gyroZ){
	float totalX = 0;
	float totalY = 0;
	float totalZ = 0;
	int i_gyro = 0;
	
	//Save the data from the accelerometer
	for(i_gyro; i_gyro < sample_number; i_gyro++){
		l3g4200d_getrawdata( &gxraw, &gyraw, &gzraw) ;
		totalX += gxraw[0];
		totalY += gyraw[0];
		totalZ += gzraw[0] ;
		delay1(13);       // delay1 function already exists in the code which pulls out raw data from sensors
	}
	
	//Get average of the values
	totalX = totalX/sample_number;
	totalY = totalY/sample_number;
	totalZ = totalZ/sample_number;
	
	/* The conversion rates for the 16 bit raw data go as follows:
	range (+/- dps)	conversion factor (mdps/digit)	final value (dps)
	245		8.75				0.00875 * raw
	500		17.50				0.01750 * raw
	2000		70.00				0.07000 * raw
	Therefore, multiply raw data with 0.07 to get answer in terms of degrees per second 
	and substract by 2000 to scale it to +/- 2000 dps. {Note that you might need to multiply 
	with invertAxes according to angular moment being being in clockwise or anti-clockwise direction} */
	totalX = (totalX*0.07) - 2000 ;
	totalY = (totalY*0.07) - 2000 ;
	totalZ = (totalZ*0.07) - 2000 ;
	// invertAxes = -1;
	
	*gyroX = totalX ;
	*gyroY = totalY ;
	*gyroZ = totalZ ;
	
}


// Computes the orientation of an object using both gyro and aceelerometer data in normalized vector form
void OrientationUsingGyroAccelFusion(float* orientX, float* orientY, float* orientZ){
	float accelX, accelY, accelZ ;
	float gyroX, gyroY, gyroZ;
	float RxEst, RyEst, RzEst;
	
	float Racc[3],Rgyro[3];
	float modulusRacc, modulusRgyro, modulusREst;
	float wGyro = 5.2;
	
	// Calculate normalized accelerometer values
	float sqAccelX = pow(accelX,2);
	float sqAccelY = pow(accelY,2);
	float sqAccelZ = pow(accelZ,2);
	float sumsqAccel = sqAccelX + sqAccelY + sqAccelZ;
	modulusRacc = sqrt(sumsqAccel);
	
	accelX = accelX / modulusRacc;
	accelY = accelY / modulusRacc;
	accelZ = accelZ / modulusRacc;
	
	//Racc = [accelX, accelY, accelZ];
	
	// Calculate normalized gyroscope values
	float sqGyroX = pow(gyroX,2);
	float sqGyroY = pow(gyroY,2);
	float sqGyroZ = pow(gyroZ,2);
	float sumsqGyro = sqGyroX + sqGyroY + sqGyroZ;
	modulusRgyro = sqrt(sumsqGyro);
	
	gyroX = gyroX / modulusRgyro;
	gyroY = gyroY / modulusRgyro;
	gyroZ = gyroZ / modulusRgyro;
	
	//Rgyro = [gyroX, gyroY, gyroZ];
	
	/* wGyro tells us how much we trust our gyro compared to our accelerometer. 
	This value can be chosen experimentally usually values between 5.20 will trigger good results.
	The main difference of this algorithm from Kalman filter is that this weight is relatively fixed , 
	whereas in Kalman filter the weights are constantly being updated.*/
	RxEst = (accelX + gyroX*wGyro) / (1 + wGyro);
	RyEst = (accelY + gyroY*wGyro) / (1 + wGyro);
	RzEst = (accelZ + gyroZ*wGyro) / (1 + wGyro);
	
	modulusREst = sqrt( pow(RxEst,2)+pow(RyEst,2)+pow(RzEst,2) );
	
	RxEst = RxEst / modulusREst;
	RyEst = RyEst / modulusREst;
	RxEst = RzEst / modulusREst;
	
	*orientX = RxEst;
	*orientY = RyEst;
	*orientZ = RzEst;	
	
}


//Function to return current elevation from the accelerometer in degrees as a float
float getElevationAcell(float* Elevation){
	float accelX, accelY, accelZ;
	*Elevation = atan2( accelZ, (sqrt( pow_i(accelX,2) + (180/PIE)*pow_i(accelY,2))) );

}


//Function to return the magnetic azimuth from the magnetometer as a float in degrees
float getAzimuthMag(float* Azimuth){
	float magX, magY, magZ;
	float Elevation;
	float elevationRad;
	
	elevationRad = (Elevation*PIE)/180;
	*Azimuth = (12.59*(PIE/180) + atan2(magZ, (magY*cos(elevationRad) - accelX*sin(elevationRad))) ) * (180/PIE);
	
}
