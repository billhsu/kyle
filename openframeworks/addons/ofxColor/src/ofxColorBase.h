#pragma once

#include "ofxColorShared.h"
#include "ofxColorMath.h"
#include "ofxColorConversion.h"
#include "ofxColorMatching.h"

namespace ofxColor {

using namespace ofxColor;

// All values are inclusive.
static ofxColorMode globalMode = OF_COLOR_RGB;
static float globalMaxRgb[3] = {1, 1, 1};
static float globalMinRgb[3] = {0, 0, 0};
static float globalMaxHsv[3] = {1, 1, 1};
static float globalMinHsv[3] = {0, 0, 0};
static float globalMinAlpha = 0;
static float globalMaxAlpha = 1;

static void setMode(ofxColorMode mode) {
	globalMode = mode;
}

static void setRange(const float max[3] = globalMaxRgb, const float min[3] = globalMinRgb) {
	set(globalMaxRgb, max);
	set(globalMinRgb, min);
	set(globalMaxHsv, max);
	set(globalMinHsv, min);
}
static void setRange(
	float maxx = globalMaxRgb[0], float maxy = globalMaxRgb[1], float maxz = globalMaxRgb[2],
	float minx = globalMinRgb[0], float miny = globalMinRgb[1], float minz = globalMinRgb[2]) {
	set(globalMaxRgb, maxx, maxy, maxz);
	set(globalMinRgb, minx, maxy, maxz);
	set(globalMaxHsv, maxx, maxy, maxz);
	set(globalMinHsv, minx, maxy, maxz);
}
static void setRgbRange(const float max[3] = globalMaxRgb, const float min[3] = globalMinRgb) {
	set(globalMaxRgb, max);
	set(globalMinRgb, min);
}
static void setRgbRange(
	float maxRed = globalMaxRgb[0], float maxGreen = globalMaxRgb[1], float maxBlue = globalMaxRgb[2],
	float minRed = globalMinRgb[0], float minGreen = globalMinRgb[1], float minBlue = globalMinRgb[2]) {
	set(globalMaxRgb, maxRed, maxGreen, maxBlue);
	set(globalMinRgb, minRed, minGreen, minBlue);
}
static void setHsvRange(const float max[3] = globalMaxRgb, const float min[3] = globalMinRgb) {
	set(globalMaxHsv, max);
	set(globalMinHsv, min);
}
static void setHsvRange(
	float maxHue = globalMaxHsv[0], float maxSaturation = globalMaxHsv[1], float maxValue = globalMaxHsv[2],
	float minHue = globalMaxHsv[0], float minSaturation = globalMaxHsv[1], float minValue = globalMaxHsv[2]) {
	set(globalMaxHsv, maxHue, maxSaturation, maxValue);
	set(globalMinHsv, minHue, minSaturation, minValue);
}
static void setAlphaRange(float max = globalMaxAlpha, float min = globalMinAlpha) {
	globalMaxAlpha = max;
	globalMinAlpha = min;
}

template<typename T>
class ofxColorBase {
protected:
	union {
		struct {
			T red, green, blue;
		};
		T rgb[3];
	};
	union {
		struct {
			T hue, saturation, value;
		};
		T hsv[3];
	};
	T alpha;

	ofxColorMode mode;
	float maxRgb[3];
	float minRgb[3];
	float maxHsv[3];
	float minHsv[3];
	float minAlpha, maxAlpha;

	bool dirty;

	T* getCurrent() {
		switch(mode) {
			case OF_COLOR_RGB:
				return rgb;
			break;
			case OF_COLOR_HSV:
				return hsv;
			break;
		}
	}
	float* getCurrentMax() const {
		switch(mode) {
			case OF_COLOR_RGB:
				return maxRgb;
			break;
			case OF_COLOR_HSV:
				return maxHsv;
			break;
		}
	}
	float* getCurrentMin() const {
		switch(mode) {
			case OF_COLOR_RGB:
				return minRgb;
			break;
			case OF_COLOR_HSV:
				return minHsv;
			break;
		}
	}

public:
	// constructors
	ofxColorBase(ofxColorMode mode = globalMode) {
		reset(mode);
	}
	ofxColorBase(T x, T y, T z, ofxColorMode mode = globalMode) {
		reset(mode);
		set(x, y, z);
	}
	ofxColorBase(T x, T y, T z, T alpha, ofxColorMode mode = globalMode) {
		reset(mode);
		set(x, y, z, alpha);
	}

	// conversion/transformation
	ofxColorBase<T>& update() {
		if(dirty) {
			float nrgb[3];
			float nhsv[3];
			switch(mode) {
				case OF_COLOR_RGB:
					ofxColor::normalize(rgb, nrgb, minRgb, maxRgb);
					convertRgbToHsv(nrgb, nhsv);
					ofxColor::unnormalize(nhsv, hsv, minHsv, maxHsv);
				break;
				case OF_COLOR_HSV:
					ofxColor::normalize(hsv, nhsv, minHsv, maxHsv);
					convertHsvToRgb(nhsv, nrgb);
					ofxColor::unnormalize(nrgb, rgb, minRgb, maxRgb);
				break;
			}
			dirty = false;
		}
		return *this;
	}
	ofxColorBase<T> getNormalized() {
		return ofxColorBase<T>(*this).normalize();
	}
	void getNormalized(float normalized[3]) {
		ofxColor::normalize(getCurrent(), normalized, getCurrentMin(), getCurrentMax());
	}
	void getRgbNormalized(float normalized[3]) {
		ofxColor::normalize(rgb, normalized, minRgb, maxRgb);
	}
	void getHsvNormalized(float normalized[3]) {
		ofxColor::normalize(hsv, normalized, minHsv, maxHsv);
	}
	ofxColorBase<T>& normalize() {
		normalizeSelf(rgb, minRgb, maxRgb);
		set(minRgb, 0);
		set(maxRgb, 1);
		normalizeSelf(hsv, minHsv, maxHsv);
		set(minHsv, 0);
		set(maxHsv, 1);
		return *this;
	}
	ofxColorBase<T> getUnnormalized() {
		return ofxColorBase<T>(*this).unnormalize();
	}
	ofxColorBase<T>& unnormalize() {
		return unnormalize(getCurrentMin(), getCurrentMax());
	}
	ofxColorBase<T>& unnormalize(const T min[3], const T max[3]) {
		unnormalizeSelf(getCurrent(), min, max);
		return *this;
	}
	ofxColorBase<T>& setFromNormalized(const float normalized[3]) {
		set(normalized);
		unnormalize();
	}

	// set multiple attributes at once
	ofxColorBase<T>& reset(ofxColorMode mode = globalMode) {
		setMode(mode);
		ofxColor::set(minRgb, globalMinRgb);
		ofxColor::set(maxRgb, globalMaxRgb);
		ofxColor::set(minHsv, globalMinHsv);
		ofxColor::set(maxHsv, globalMaxHsv);
		minAlpha = globalMinAlpha;
		maxAlpha = globalMaxAlpha;
		ofxColor::set(rgb, minRgb);
		ofxColor::set(hsv, minHsv);
		setAlpha((T) maxAlpha);
		dirty = false;
		return *this;
	}
	ofxColorBase<T>& set(T x, T y, T z, T alpha) {
		setAlpha(alpha);
		return set(x, y, z);
	}
	ofxColorBase<T>& set(T x, T y, T z) {
		ofxColor::set(getCurrent(), x, y, z);
		dirty = true;
		return *this;
	}
	template<typename S, typename A>
	ofxColorBase<T>& set(const S x[3], A alpha) {
		setAlpha(alpha);
		return set(x);
	}
	template<typename S>
	ofxColorBase<T>& set(const S x[3]) {
		ofxColor::set(getCurrent(), x);
		dirty = true;
		return *this;
	}

	// set range (will reset anything to global that is not specified)
	ofxColorBase<T>& setRange(const float max[3] = globalMaxRgb, const float min[3] = globalMinRgb) {
		unnormalize(min, max);
		dirty = true;
		ofxColor::set(maxRgb, max);
		ofxColor::set(minRgb, min);
		ofxColor::set(maxHsv, max);
		ofxColor::set(minHsv, min);
	}
	ofxColorBase<T>& setRange(
		float maxx = globalMaxRgb[0], float maxy = globalMaxRgb[1], float maxz = globalMaxRgb[2],
		float minx = globalMinRgb[0], float miny = globalMinRgb[1], float minz = globalMinRgb[2]) {
		float min[3] = {minx, miny, minz};
		float max[3] = {maxx, maxy, maxz};
		map(min, max);
		dirty = true;
		ofxColor::set(maxRgb, max);
		ofxColor::set(minRgb, min);
		ofxColor::set(maxHsv, max);
		ofxColor::set(minHsv, min);
	}
	ofxColorBase<T>& setRgbRange(const float max[3] = globalMaxRgb, const float min[3] = globalMinRgb) {
		if(mode == OF_COLOR_RGB)
			map(min, max);
		else
			dirty = true;
		ofxColor::set(maxRgb, max);
		ofxColor::set(minRgb, min);
	}
	ofxColorBase<T>& setRgbRange(
		float maxRed = globalMaxRgb[0], float maxGreen = globalMaxRgb[1], float maxBlue = globalMaxRgb[2],
		float minRed = globalMinRgb[0], float minGreen = globalMinRgb[1], float minBlue = globalMinRgb[2]) {
		float min[3] = {minRed, minGreen, minBlue};
		float max[3] = {maxRed, maxGreen, maxBlue};
		if(mode == OF_COLOR_RGB)
			unnormalize(min, max);
		else
			dirty = true;
		ofxColor::set(maxRgb, max);
		ofxColor::set(minRgb, min);
	}
	ofxColorBase<T>& setHsvRange(const float max[3] = globalMaxRgb, const float min[3] = globalMinRgb) {
		if(mode == OF_COLOR_HSV)
			map(min, max);
		else
			dirty = true;
		ofxColor::set(maxHsv, max);
		ofxColor::set(minHsv, min);
	}
	ofxColorBase<T>& setHsvRange(
		float maxHue = globalMaxHsv[0], float maxSaturation = globalMaxHsv[1], float maxValue = globalMaxHsv[2],
		float minHue = globalMaxHsv[0], float minSaturation = globalMaxHsv[1], float minValue = globalMaxHsv[2]) {
		float min[3] = {minHue, minSaturation, minValue};
		float max[3] = {maxHue, maxSaturation, maxValue};
		if(mode == OF_COLOR_HSV)
			map(min, max);
		else
			dirty = true;
		ofxColor::set(maxHsv, max);
		ofxColor::set(minHsv, min);
	}
	ofxColorBase<T>& setAlphaRange(float max = globalMaxAlpha, float min = globalMinAlpha) {
		maxAlpha = max;
		minAlpha = min;
	}

	// set attributes directly
	ofxColorBase<T>& setMode(ofxColorMode mode) {
		update();
		this->mode = mode;
		return *this;
	}
	ofxColorBase<T>& setRed(T red) {
		this->red = red;
		dirty = true;
		return *this;
	}
	ofxColorBase<T>& setGreen(T green) {
		this->green = green;
		dirty = true;
		return *this;
	}
	ofxColorBase<T>& setBlue(T blue) {
		this->blue = blue;
		dirty = true;
		return *this;
	}
	ofxColorBase<T>& setHue(T hue) {
		this->hue = hue;
		dirty = true;
		return *this;
	}
	ofxColorBase<T>& setSaturation(T saturation) {
		this->saturation = saturation;
		dirty = true;
		return *this;
	}
	ofxColorBase<T>& setValue(T value) {
		this->value = value;
		dirty = true;
		return *this;
	}
	ofxColorBase<T>& setAlpha(T alpha) {
		this->alpha = alpha;
		return *this;
	}

	// getters
	ofxColorMode getMode() const {
		return mode;
	}
	T getRed() {
		update();
		return red;
	}
	T getGreen() {
		update();
		return green;
	}
	T getBlue() {
		update();
		return blue;
	}
	T getHue() {
		update();
		return hue;
	}
	T getSaturation() {
		update();
		return saturation;
	}
	T getValue() {
		update();
		return value;
	}
	T getAlpha() const {
		return alpha;
	}

	const float* getMaxRgb() const {
		return maxRgb;
	}
	const float* getMinRgb() const {
		return minRgb;
	}
	const float* getMaxHsv() const {
		return maxHsv;
	}
	const float* getMinHsv() const {
		return minHsv;
	}
	const T* getRgb() {
		update();
		return rgb;
	}
	const T* getHsv() {
		update();
		return hsv;
	}

	// utilities
	ofxColorBase<T>& clamp() {
		float normalized[3];
		getNormalized(normalized);
		switch(mode) {
			case OF_COLOR_RGB:
				ofxColor::clampRgbSelf(normalized);
			break;
			case OF_COLOR_HSV:
				ofxColor::clampHsvSelf(normalized);
			break;
		}
		setFromNormalized(normalized);
		return *this;
	}
	template<typename A>
	ofxColorBase<T>& lerp(ofxColorBase<A>& target, float amount) {
		float normalizedSource[3];
		float normalizedTarget[3];
		getNormalized(normalizedSource);
		target.getNormalized(normalizedTarget);
		ofxColor::lerp(normalizedSource, normalizedTarget, amount, normalizedSource);
		setFromNormalized(normalizedSource);
		return *this;
	}
	template<typename TMIN, typename TMAX>
	ofxColorBase<T>& map(
		ofxColorBase<TMIN>& toMin, ofxColorBase<TMAX>& toMax) {
		ofxColor::map(getCurrent(), getCurrentMin(), getCurrentMax(), toMin, toMax, getCurrent());
		dirty = true;
	}
	template<typename FMIN, typename FMAX, typename TMIN, typename TMAX>
	ofxColorBase<T>& map(
		ofxColorBase<FMIN>& fromMin, ofxColorBase<FMAX>& fromMax,
		ofxColorBase<TMIN>& toMin, ofxColorBase<TMAX>& toMax) {
		ofxColor::map(getCurrent(), fromMin, fromMax, toMin, toMax, getCurrent());
		dirty = true;
		return *this;
	}
	ofxColorBase<T> getClamped() const {
		return ofxColorBase<T>(*this).clamp();
	}
	template<typename A>
	ofxColorBase<T> getLerp(ofxColorBase<A>& target, float amount) const {
		return ofxColorBase<T>(*this).lerp(target, amount);
	}
	template<typename FMIN, typename FMAX, typename TMIN, typename TMAX>
	ofxColorBase<T>& getMap(
		ofxColorBase<FMIN>& fromMin, ofxColorBase<FMAX>& fromMax,
		ofxColorBase<TMIN>& toMin, ofxColorBase<TMAX>& toMax) {
		return ofxColorBase<T>(*this).map(fromMin, fromMax, toMin, toMax);
	}

	// copy constructors, assignment operators

	// matching
	template<typename A>
	float distance(ofxColorBase<A>& target) {
		float sourceNormalized[3];
		float targetNormalized[3];
		getNormalized(sourceNormalized);
		switch(mode) {
			case OF_COLOR_RGB:
				target.getRgbNormalized(targetNormalized);
				return ofxColor::distanceRgb(sourceNormalized, targetNormalized);
			break;
			case OF_COLOR_HSV:
				target.getHsvNormalized(targetNormalized);
				return ofxColor::distanceHsv(sourceNormalized, targetNormalized);
			break;
		}
	}
	template<typename A>
	bool match(ofxColorBase<A>& target, const float threshold[3]) {
		float normalizedSource[3];
		float normalizedTarget[3];
		getNormalized(normalizedSource);
		switch(mode) {
			case OF_COLOR_RGB:
				target.getRgbNormalized(normalizedTarget);
				return matchRgb(normalizedSource, normalizedTarget, threshold);
				break;
			case OF_COLOR_HSV:
				target.getHsvNormalized(normalizedTarget);
				return matchHsv(normalizedSource, normalizedTarget, threshold);
			break;
		}
	}

	// arithmetic operators

	friend ostream &operator<<(ostream &out, ofxColorBase<T>& color) {
		out << "(";
		switch(color.getMode()) {
			case OF_COLOR_RGB:
				out << (float) color.getRed() << ", " << (float) color.getGreen() << ", " << (float) color.getBlue();
			break;
			case OF_COLOR_HSV:
				out << (float) color.getHue() << ", " << (float) color.getSaturation() << ", " << (float) color.getValue();
			break;
		}
		out << ", " << (float) color.getAlpha() << ")";
		return out;
	}
};

}
