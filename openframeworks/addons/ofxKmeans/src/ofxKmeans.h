#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxColor.h"
#include "cxcore.h"

class Cluster {
public:
	void startMean() {
		xSum = 0;
		ySum = 0;
		rSum = 0;
		gSum = 0;
		bSum = 0;
		n = 0;
	}
	void addMean(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
		xSum += x;
		ySum += y;
		rSum += r;
		gSum += g;
		bSum += b;
		n++;
	}
	void finishMean() {
		center.x = (float) xSum / n;
		center.y = (float) ySum / n;
		color.red =	(float) rSum / n;
		color.green = (float) gSum / n;
		color.blue = (float) bSum / n;
		color.updateHsv();
	}
	void startDeviation() {
		xDeviation = 0;
		yDeviation = 0;
		rDeviation = 0;
		gDeviation = 0;
		bDeviation = 0;
		n = 0;
	}
	void addDeviation(float x, float y, float r, float g, float b) {
		x -= center.x;
		y -= center.y;
		r -= color.red;
		g -= color.green;
		b -= color.blue;

		x *= x;
		y *= y;
		r *= r;
		g *= g;
		b *= b;

		xDeviation += x;
		yDeviation += y;
		rDeviation += r;
		gDeviation += g;
		bDeviation += b;
		n++;
	}
	void finishDeviation() {
		centerDeviation.x = sqrt((float) xDeviation / n);
		centerDeviation.y = sqrt((float) yDeviation / n);
		colorDeviation.red =	sqrt((float) rDeviation / n);
		colorDeviation.green = sqrt((float) gDeviation / n);
		colorDeviation.blue = sqrt((float) bDeviation / n);
		colorDeviation.updateHsv();
	}
	unsigned int size() {
		return n;
	}
	bool operator!=(const Cluster& c) {
		return !((*this) == c);
	}
	bool operator==(const Cluster& c) {
		return
			center == c.center &&
			color == c.color;
	}
	friend bool operator<(const Cluster& x, const Cluster& y) {
		return x.n < y.n;
	}
	ofPoint center, centerDeviation;
	ofxColor color, colorDeviation;
protected:
	unsigned long xSum, ySum;
	unsigned long rSum, gSum, bSum;
	float xDeviation, yDeviation;
	float rDeviation, gDeviation, bDeviation;
	unsigned int n;
};

class ofxKmeans {
public:
	vector<Cluster> clusters;
	vector<ofxCvGrayscaleImage> thresholds; // should be moved to Cluster
	int maxBlobs;
	float minBlobRatio, maxBlobRatio;

	ofxKmeans(int maxBlobs = 3, float maxBlobRatio = .75, float minBlobRatio = .1) {
		this->maxBlobs = maxBlobs;
		this->maxBlobRatio = maxBlobRatio;
		this->minBlobRatio = minBlobRatio;
	}
	vector<int> clusterPoints(vector<ofPoint>& data, unsigned nClusters = 3) {
		int nPoints = data.size();
		CvMat* points = cvCreateMat(nPoints, 1, CV_32FC2);
		float* dataptr = (float*) points->data.ptr;
		for(int i = 0; i < nPoints; i++) {
			*(dataptr++) = data[i].x;
			*(dataptr++) = data[i].y;
		}

		CvMat* labels = cvCreateMat(points->rows, 1, CV_32SC1);
		cvKMeans2(
			points, // data to cluster
			nClusters, // how many clusters to use
			labels,  // resulting labels on clusters
			cvTermCriteria( // conditions for stopping the clustering
				CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0));

		vector<int> result;
		result.resize(nPoints);

		int* labelptr = (int*) labels->data.ptr;
		for(int i = 0; i < labels->rows; i++)
			result[i] = *(labelptr++);

		cvReleaseMat(&points);
		cvReleaseMat(&labels);

		return result;
	}

	void adaptiveClusterColors(ofxCvColorImage& ofxImg, unsigned maxClusters = 6, bool blobTracking = false, float minClusterSize = .1, unsigned minClusters = 2) {
		int bestClusters = 0;
		float bestDeviation = 0;
		float totalArea = ofxImg.getWidth() * ofxImg.getHeight();
		// minimize average standard deviation of cluster colors
		for(int nClusters = minClusters; nClusters <= maxClusters; nClusters++) {
			clusterColors(ofxImg, nClusters, false);
			float deviation = getAverageColorDeviation();

			bool wellSized = true;
			for(int i = 0; i < clusters.size(); i++)
				if(clusters[i].size() < minClusterSize * totalArea)
					wellSized = false;

			if((deviation < bestDeviation && wellSized)
				|| bestClusters == 0) {
				bestClusters = nClusters;
				bestDeviation = deviation;
			}
		}
		clusterColors(ofxImg, bestClusters, blobTracking);
	}

	void clusterColors(ofxCvColorImage& ofxImg, unsigned nClusters = 3, bool blobTracking = false) {
		ofRectangle roi = ofxImg.getROI();
		float w = roi.width;
		float h = roi.height;
		unsigned nPoints = (unsigned) (w * h);
		IplImage* img = ofxImg.getCvImage();

		// copy wxhxc matrix into a single (wxh)xc unsigned char array
		CvMat* linear = cvCreateMat(nPoints, 1, CV_8UC3);
		unsigned char* dataptr = (unsigned char*) linear->data.ptr;
		// starts the imgptr at the roi x,y
		unsigned char* imgptr = (unsigned char*) (
			img->imageData +
			((int) roi.y) * img->widthStep +
			((int) roi.x) * img->nChannels);
		for(int y = 0; y < h; y++) {
			for(int x = 0; x < w; x++) {
				*(dataptr++) = imgptr[3 * x + 0];
				*(dataptr++) = imgptr[3 * x + 1];
				*(dataptr++) = imgptr[3 * x + 2];
			}
			imgptr += img->widthStep;
		}

		// convert the unsigned chars to 32-bit floats
		CvMat* points = cvCreateMat(nPoints, 1, CV_32FC3);
		cvConvert(linear, points);

		// prepare the label values and call cvKMeans2
		CvMat* labels = cvCreateMat(points->rows, 1, CV_32SC1);
		cvKMeans2(
			points, // data to cluster
			nClusters, // how many clusters to use
			labels,  // resulting labels on clusters
			cvTermCriteria( // conditions for stopping the clustering
				CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,
					10, // max number of iterations
					1.0)); // eps, accuracy of match

		// clear out old clusters, and fill them up with new ones
		clusters.clear();
		clusters.resize(nClusters);
		int x, y;
		int* labelptr;
		unsigned char* linearptr;

		for(int i = 0; i < nClusters; i++)
			clusters[i].startMean();

		// build up means
		x = 0, y = 0;
		labelptr = (int*) labels->data.ptr;
		linearptr = (unsigned char*) linear->data.ptr;
		for(int i = 0; i < labels->rows; i++) {
			clusters[*labelptr].addMean(x, y, linearptr[0], linearptr[1], linearptr[2]);
			labelptr++;
			linearptr += linear->step;
			x++;
			if(x == w) {
				x = 0;
				y++;
			}
		}

		// done building means, average and prep std dev
		for(int i = 0; i < nClusters; i++) {
			clusters[i].finishMean();
			clusters[i].startDeviation();
		}

		// build up std dev
		x = 0, y = 0;
		labelptr = (int*) labels->data.ptr;
		linearptr = (unsigned char*) linear->data.ptr;
		for(int i = 0; i < labels->rows; i++) {
			clusters[*labelptr].addDeviation(x, y, linearptr[0], linearptr[1], linearptr[2]);
			labelptr++;
			linearptr += linear->step;
			x++;
			if(x == w) {
				x = 0;
				y++;
			}
		}

		// done building std dev, average and sqrt
		for(int i = 0; i < nClusters; i++)
			clusters[i].finishDeviation();

		// if we're blob tracking, prepare threshold images
		if(blobTracking) {
			thresholds.resize(nClusters);
			for(int i = 0; i < nClusters; i++) {
				thresholds[i].clear();
				thresholds[i].allocate(ofxImg.width, ofxImg.height);
			}
			for(int i = 0; i < nClusters; i++) {
				img = thresholds[i].getCvImage();
				imgptr = (unsigned char*) img->imageData;
				labelptr = (int*) labels->data.ptr;
				for(int y = 0; y < h; y++) {
					for(int x = 0; x < w; x++)
						if(*(labelptr++) == i)
							imgptr[x] = 0xff;
						else
							imgptr[x] = 0x00;
					imgptr += img->widthStep;
				}
			}
		}

		cvReleaseMat(&linear);
		cvReleaseMat(&points);
		cvReleaseMat(&labels);
	}

	float getAverageColorDeviation() {
		float deviation = 0;
		for(int i = 0; i < clusters.size(); i++) {
			deviation += clusters[i].colorDeviation.red;
			deviation += clusters[i].colorDeviation.green;
			deviation += clusters[i].colorDeviation.blue;
		}
		return deviation / (clusters.size() * 3);
	}

	void sortClusters() {
		std::sort(clusters.begin(), clusters.end());
	}
	void reverseClusters() {
		std::reverse(clusters.begin(), clusters.end());
	}
	ofxColor& getDominantColor(ofxCvColorImage& ofxImg, int nClusters = 3) {
		clusterColors(ofxImg, nClusters);
		sortClusters();
		return clusters.back().color;
	}
	ofxColor& getAdaptiveDominantColor(ofxCvColorImage& ofxImg, int maxClusters = 6, float minClusterSize = .1, int minClusters = 2) {
		adaptiveClusterColors(ofxImg, maxClusters, false, minClusterSize, minClusters);
		sortClusters();
		return clusters.back().color;
	}
	vector<ofxColor> getColors() {
		vector<ofxColor> colors;
		for(int i = 0; i < clusters.size(); i++)
			colors.push_back(clusters[i].color);
		return colors;
	}
	ofxCvContourFinder getBlobs(int i) {
		float totalArea = thresholds[i].getWidth() * thresholds[i].getHeight();
		float maxArea = totalArea * maxBlobRatio;
		float minArea = totalArea * minBlobRatio;
		ofxCvContourFinder contourFinder;
		contourFinder.findContours(thresholds[i], (int) minArea, (int) maxArea, maxBlobs, false);
		return contourFinder;
	}
};
