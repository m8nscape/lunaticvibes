#include "ruleset_network.h"
#include "ruleset_bms_network.h"

std::vector<unsigned char> vRulesetNetwork::packInit(std::shared_ptr<RulesetBase> local)
{
	if (auto p = std::dynamic_pointer_cast<RulesetBMS>(local); p != nullptr)
	{
		return RulesetBMSNetwork::packInit(p);
	}
	return {};
}

std::vector<unsigned char> vRulesetNetwork::packFrame(std::shared_ptr<RulesetBase> local)
{
	if (auto p = std::dynamic_pointer_cast<RulesetBMS>(local); p != nullptr)
	{
		return RulesetBMSNetwork::packFrame(p);
	}
	return {};
}

