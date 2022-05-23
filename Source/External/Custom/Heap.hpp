#pragma once
#include <array>
#include <algorithm>


namespace CommonUtilities
{
	enum class HeapType
	{
		Max, Min
	};
	template <class T, HeapType aHeapType = HeapType::Max>
	class Heap
	{
#define LEFT_CHILD(i)  (2 * i) + 1
#define RIGHT_CHILD(i) (2 * i) + 2
#define PARENT(i) (i - 1) / 2
	public:
		Heap() = default;
		//returnerar antal element i heapen
		int GetSize() const
		{
			return static_cast<int>(myBuffer.size());
		}
		//lägger till elementet i heapen
		void Enqueue(const T& aElement)
		{
			myBuffer.push_back(aElement);

			int current = static_cast<int>(myBuffer.size()) - 1;

			bool bubbleDown = aHeapType == HeapType::Min ? myBuffer[PARENT(current)] < myBuffer[current] : myBuffer[current] < myBuffer[PARENT(current)];


			while (bubbleDown)
			{
				std::swap(myBuffer[PARENT(current)], myBuffer[current]);
				current = PARENT(current);
				bubbleDown = aHeapType == HeapType::Min ? myBuffer[PARENT(current)] < myBuffer[current] : myBuffer[current] < myBuffer[PARENT(current)];
			}

			//BubbleUp((int)myBuffer.size() - 1);
		}
		//returnerar det första elementet i heapen
		const T& GetTop() const
		{
			return myBuffer.front();
		}
		//tar bort det första elementet ur heapen och returnerar det
		T Dequeue()
		{
			if (myBuffer.size() == 0) return T();
			T topElement = GetTop();

			myBuffer[0] = myBuffer.back();
			myBuffer.pop_back();

			int i = 0;
			while (true)
			{
				int lC = LEFT_CHILD(i);
				int rC = RIGHT_CHILD(i);

				if (rC < myBuffer.size())
				{
					int largest = 0;
					switch (aHeapType)
					{
					case CommonUtilities::HeapType::Max:
						largest = myBuffer[lC] < myBuffer[rC] ? rC : lC;
						break;
					case CommonUtilities::HeapType::Min:
						largest = myBuffer[rC] < myBuffer[lC] ? rC : lC;
						break;
					}


					bool bubbleUp = aHeapType == HeapType::Min ? myBuffer[largest] < myBuffer[i] : myBuffer[i] < myBuffer[largest];
					if (bubbleUp)
					{
						std::swap(myBuffer[i], myBuffer[largest]);
						i = largest;
						continue;
					}
				}
				else if (lC < myBuffer.size())
				{
					bool bubbleUp = aHeapType == HeapType::Min ? myBuffer[lC] < myBuffer[i] : myBuffer[i] < myBuffer[lC];
					if (bubbleUp)
					{
						std::swap(myBuffer[i], myBuffer[lC]);
					}
				}
				break;

			}



			return topElement;
		}

		bool Contains(T aValue)
		{
			return std::find(myBuffer.begin(), myBuffer.end(), aValue) != myBuffer.end();
		}
	
		void Reset()
		{
			myBuffer.clear();
		}

		void Sort()
		{
			int i = 0;
			while (true)
			{
				int lC = LEFT_CHILD(i);
				int rC = RIGHT_CHILD(i);

				if (rC < myBuffer.size())
				{
					int largest = 0;
					switch (aHeapType)
					{
					case CommonUtilities::HeapType::Max:
						largest = myBuffer[lC] < myBuffer[rC] ? rC : lC;
						break;
					case CommonUtilities::HeapType::Min:
						largest = myBuffer[rC] < myBuffer[lC] ? rC : lC;
						break;
					}


					bool bubbleUp = aHeapType == HeapType::Min ? myBuffer[largest] < myBuffer[i] : myBuffer[i] < myBuffer[largest];
					if (bubbleUp)
					{
						std::swap(myBuffer[i], myBuffer[largest]);
						i = largest;
						continue;
					}
				}
				else if (lC < myBuffer.size())
				{
					bool bubbleUp = aHeapType == HeapType::Min ? myBuffer[lC] < myBuffer[i] : myBuffer[i] < myBuffer[lC];
					if (bubbleUp)
					{
						std::swap(myBuffer[i], myBuffer[lC]);
					}
				}
				break;

			}
		}
	
	private:
		int myCurrentSize = 0;
		std::vector<T> myBuffer;

	};
}