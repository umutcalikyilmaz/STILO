#pragma once
#include <vector>
#include <span>
#include "STILO/Shared/Random.h"

namespace STILO
{
	template <typename T>
    inline int findElement(std::span<const T> list, T val, int size)
    {
        int ll = 0;
		int ul = size;		

		do
		{
			int curr = (ul + ll) / 2;

			if(list[curr] > val)
			{
				ul = curr;
			}
			else
			{
				ll = curr;
			}
		}
		while (ul - ll > 1);

		if(list[ll] > val)
		{
			return ll;
		}

		return ul;
    }
	
	inline int findPlace(std::span<const double> list, std::span<const int> inds, double val, int size)
    {
        int ul = size - 1;
		int ll = 0; 

		if(val >= list[inds[ul]])
		{
			return ul;
		}

		int diff = ul - ll;

		while(diff > 1)
		{
			int curr = diff / 2 + ll;

			if(val < list[inds[curr]])
			{
				ul = curr;
			}
			else if(val > list[inds[curr]])
			{
				ll = curr;
			}
			else
			{
				return curr;
			}

			diff = ul - ll;
		}		

		return ll;
    }

	inline int findPlace2(std::span<const double> list, std::span<const int> inds, double val, int size)
    {
        if(size == 0)
		{
			return 0;
		}
		
		int ul = size;
		int ll = 0; 
		int diff = ul - ll;		

		while(diff > 1)
		{
			int curr = diff / 2 + ll;

			if(val < list[inds[curr]])
			{
				ul = curr;
			}
			else if(val > list[inds[curr]])
			{
				ll = curr;
			}
			else
			{
				return curr;
			}

			diff = ul - ll;
		}

		if(val > list[inds[ll]])
		{
			return ul;
		}

		return ll;
    }

	inline int findMinimum(std::span<const double> list, std::span<const int> inds, double val, int size)
    {
        if(size == 0)
		{
			return 0;
		}
		
		int ul = size;
		int ll = 0; 
		int diff = ul - ll;

		while(diff > 1)
		{
			int curr = diff / 2 + ll;

			if(val < list[inds[curr]])
			{
				ul = curr;
			}
			else if(val > list[inds[curr]])
			{
				ll = curr;
			}
			else
			{
				return curr;
			}

			diff = ul - ll;
		}

		if(val > list[inds[ll]])
		{
			return ul;
		}

		return ll;
    }

	inline int findMaximum(std::span<const double> list, std::span<const int> inds, double val, int size)
    {
        if(size == 0)
		{
			return 0;
		}
		
		int ul = size;
		int ll = 0; 
		int diff = ul - ll;

		while(diff > 1)
		{
			int curr = diff / 2 + ll;

			if(val > list[inds[curr]])
			{
				ul = curr;
			}
			else if(val < list[inds[curr]])
			{
				ll = curr;
			}
			else
			{
				return curr;
			}

			diff = ul - ll;
		}

		if(val < list[inds[ll]])
		{
			return ul;
		}

		return ll;
    }
	
	inline void shift(std::span<int> list, int ind)
    {
        for(int i = 0; i < ind; i++)
		{
			list[i] = list[i + 1];
		}
    }

	template <typename T>
	inline void shiftLeft(std::span<T> list, int ind, int size)
	{
		for(int i = ind; i < size; i++)
		{
			list[i] = list[i + 1];
		}
	}
	
	template <typename T>
	inline void shiftRightPartial(std::span<T> list, int ind, int size)
	{
		for(int i = size - 1; i >= ind; i--)
		{
			list[i + 1] = list[i];
		}
	}

	template <typename T>
	inline void shiftListRight(std::vector<std::vector<T>>& list, int ind, int size)
	{
		for(int i = size - 2; i >= ind; i--)
        {
            std::swap(list[i + 1], list[i]);
        }
	}

	template <typename T>
	inline void shiftRight(std::span<T> list, int ind, int size)
    {
        for(int i = size - 2; i >= ind; i--)
		{
			list[i + 1] = list[i];
		}
    }

	inline void deleteElement(std::span<int> list, int ind, int size)
    {
        for(int i = ind; i < size - 1; i++)
        {
            list[i] = list[i + 1];
        }
    }

	inline void deleteRandom(std::span<int> list, int ind, int size)
	{
		list[ind] = list[size - 1];
	}

	inline void copy(std::span<const int> copied, std::span<int> target, int size)
    {
        for(int i = 0; i < size; i++)
		{
			target[i] = copied[i];
		}
    }

	inline void copyAsymmetric(std::span<const int> copied, std::vector<int>& target, int size)
	{
		for(int i = 0; i < size; i++)
		{
			target[i] = copied[i];
		}
	}

	inline void permute(std::span<int> list, int ind1, int ind2)
    {
        int temp = list[ind1];
		list[ind1] = list[ind2];
		list[ind2] = temp;
    }

	inline void randomize(UniformRandomIntGenerator& rnd, int& ind1, int& ind2)
    {
        ind1 = rnd.generate();

		do
		{
			ind2 = rnd.generate();
		} 
		while (ind1 == ind2);
    }

	inline void randomize(std::span<int> list, UniformRandomIntGenerator& rnd)
    {
        int ind1;
		int ind2;
		randomize(rnd, ind1, ind2);
		permute(list, ind1, ind2);
    }

	inline void randomize(std::span<int> list, UniformRandomIntGenerator& rnd1,  UniformRandomIntGenerator& rnd2)
    {
        int ind = rnd1.generate();
		int temp = list[ind];
		int newval;

		do
		{
			newval = rnd2.generate();
		}
		while (temp == newval);

		list[ind] = newval;
    }

	inline void invertRandom(std::span<int> list, int size, UniformRandomIntGenerator& rnd)
	{
		int ind1;
		int ind2;
		randomize(rnd, ind1, ind2);

		if(ind1 > ind2)
        {
            ind2 = ind2 + size;
        }

        while(ind1 < ind2)
        {
            int tempind1 = ind1 % size;
            int tempind2 = ind2 % size;
            int temp = list[tempind1];
            list[tempind1] = list[tempind2];
            list[tempind2] = temp;
            ind1++;
            ind2--;
        }
	}

	inline void insertRandom(std::span<int> list, UniformRandomIntGenerator& rnd)
	{
		int ind1;
        int ind2;
        randomize(rnd, ind1, ind2);

		int temp = list[ind1];

        if(ind1 > ind2)
        {
            shiftRight<int>(list, ind2, ind1 + 1);            
        }
        else
        {
            shiftLeft<int>(list, ind1, ind2);
        }

        list[ind2] = temp; 
	}

	inline void pointRandom(std::vector<int>& list, int ind, UniformRandomIntGenerator& rnd)
	{
		int cur = list[ind];

		do
		{
			list[ind] = rnd.generate();
		}
		while(list[ind] == cur);
	}
}