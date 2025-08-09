/*
  ==============================================================================

	ZDFLadderFilter.h
	Created: 9 Aug 2025 6:45:05am
	Author:  Jules

  ==============================================================================
*/
#include <cmath>
#include <algorithm>
#pragma once
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
namespace SynthDSP
{

	class ZDFLadderFilter
	{
	public:
		void ZDFLadderFilter::prepare(double sr, float smoothTimeMs = 5.0f);

		void ZDFLadderFilter::set(float c, float r, float d);
		void ZDFLadderFilter::reset();
		static float ZDFLadderFilter::mapNormToCutoffExp(float norm, float fs, float minHz);
		float ZDFLadderFilter::processSample(float x);
	private:
		// --- public API params (targets) ---
		double sampleRate = 44100.0;
		float cutoffNormTarget = 1000.0f / 22050.0f; // normalized 0..1 (default ~1k at 44.1k)
		float resonanceTarget = 0.5f;               // 0..1
		float driveTarget = 0.2f;               // 0..1
		float uPrev = 0.0f;   // warm-start for Newton

		// --- smoothed working params ---
		float cutoffNormSm = cutoffNormTarget;
		float resonanceSm = resonanceTarget;
		float driveSm = driveTarget;

		// smoothing (one-pole): coef = exp(-1/(tau*fs))
		float smoothCoef = 0.0f; // set in prepare()
		float invOneMinusSmooth = 1.0f; // 1 - smoothCoef

		// TPT integrator states
		float s1 = 0.0f, s2 = 0.0f, s3 = 0.0f, s4 = 0.0f;

		// --- config ---
		// clamp states if you ever hit a weird corner (cheap guard)
		static constexpr bool kClampStates = true;
		static constexpr float kStateLimit = 20.0f; // tweak if needed


	};

} // namespace SynthDSP
