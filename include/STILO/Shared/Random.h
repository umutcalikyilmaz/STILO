#pragma once
#include <random>
#include <cmath>
#include <cstdlib>
#include <chrono>

namespace STILO
{
	class RandomNumberGenerator
	{
	protected:

		std::minstd_rand generator;

		RandomNumberGenerator()
			:  generator(std::random_device{}()) { }
	};

	template<class GeneratorType>
	class RandomIntGenerator : public RandomNumberGenerator
	{
	public:

		int generate()
		{
			return static_cast<GeneratorType*>(this)->generate();
		}
	};

	template<class GeneratorType>
	class RandomDoubleGenerator : public RandomNumberGenerator
	{
	public:
		
		double generate()
		{
			return static_cast<GeneratorType*>(this)->generate();
		}
	};


	class UniformRandomDoubleGenerator : public RandomDoubleGenerator<UniformRandomDoubleGenerator>
	{
	public:

		UniformRandomDoubleGenerator(double min, double max) : uniformDistribution_(min, max) { }

		double generate()
		{
			return uniformDistribution_(generator);
		}

	private:

		std::uniform_real_distribution<double> uniformDistribution_;
	};

	class NormalRandomDoubleGenerator : public RandomDoubleGenerator<NormalRandomDoubleGenerator>
	{
	public:

		NormalRandomDoubleGenerator(double mean, double std) : normalDistribution_(mean, std) { }

		double generate()
		{
			return normalDistribution_(generator);
		}

	private:

		std::normal_distribution<double> normalDistribution_;	
	};

	class UniformRandomIntGenerator : public RandomIntGenerator<UniformRandomIntGenerator>
	{
	public: 

		UniformRandomIntGenerator(int min, int max) : uniformDistribution_(min, max) { }

		int generate()
		{
			return uniformDistribution_(generator);
		}

	private:
	
		std::uniform_int_distribution<int> uniformDistribution_;
	};

	class NormalRandomIntGenerator : public RandomIntGenerator<NormalRandomIntGenerator>
	{
	public:
		
		NormalRandomIntGenerator(double mean, double std, int min, int max)
			: normalDistribution_(mean, std),
			  mean_(mean),
			  min_(min), 
			  max_(max) {}

		int generate()
		{
			int rand = min_ + (int)(0.5 + abs(normalDistribution_(generator)));
			return rand < max_ ? rand : max_;
		}

		void changeStd(double std)
		{
			normalDistribution_.param(std::normal_distribution<double>::param_type(mean_, std));
		}

	private:

		std::normal_distribution<double> normalDistribution_;
		double mean_;
		int min_;
		int max_;
	};
	
	class CauchyRandomIntGenerator : public RandomIntGenerator<CauchyRandomIntGenerator>
	{
	public:

		CauchyRandomIntGenerator(double mean, double gamma, int min, int max)
			: cauchyDistribution_(mean, gamma),
			  mean_(mean),
			  min_(min),
			  max_(max) { }

		int generate()
		{
			int rand = min_ + (int)(0.5 + abs(cauchyDistribution_(generator)));
			return rand < max_ ? rand : max_;
		}

		void changeGamma(double gamma)
		{
			cauchyDistribution_.param(std::cauchy_distribution<double>::param_type(mean_, gamma));
		}

	private:

		std::cauchy_distribution<double> cauchyDistribution_;
		double mean_;
		int min_;
		int max_;
	};	
}