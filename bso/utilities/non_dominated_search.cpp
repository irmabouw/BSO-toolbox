#ifndef BSO_NON_DOMINATED_SEARCH_CPP
#define BSO_NON_DOMINATED_SEARCH_CPP

#include <algorithm>

namespace bso { namespace utilities {
	
std::vector<data_point> non_dominated_search(const std::vector<data_point>& data)
{
	std::vector<data_point> dataCopy = data;
	return non_dominated_search(dataCopy);
}
std::vector<data_point> non_dominated_search(std::vector<data_point>& data)
{
	std::vector<data_point> sorted;
	std::sort(data.begin(), data.end());
	for (const auto& i : data)
	{
		bool nonDominated = true;
		for (auto j = sorted.begin(); j != sorted.end(); ++j)
		{
			if (j->dominates(i))
			{
				nonDominated = false;
				break;
			}
			else if (j->isDominatedBy(i)) j = sorted.erase(j);
		}
		if (nonDominated) sorted.push_back(i);
	}
	return sorted;
}
	
} // namespace utilities
} // namespace bso

#endif // BSO_NON_DOMINATED_SEARCH_CPP