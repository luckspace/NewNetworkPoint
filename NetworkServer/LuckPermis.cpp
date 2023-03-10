#include "LuckPermis.hpp"

bool luck::permis::PermissionNode::VerifyPermission(const std::string& _permis)
{
	if (node.empty())
		return false;

	auto node_list = SplitPermission(_permis);

	auto node_ptr = this;
	for (const auto& i : node_list)
	{
		if (node_ptr->node.empty())
			return true;
		auto item = node_ptr->node.find(i);
		if (item == node_ptr->node.end())
			return false;
		else
			node_ptr = &item->second;
	}
	if (node_ptr->node.empty())
		return true;
	return false;
}

void luck::permis::PermissionNode::InsertPermission(const std::string& _permis)
{
	auto node_list = SplitPermission(_permis);

	auto node_ptr = this;
	for (const auto& i : node_list)
	{
		auto item = node_ptr->node.find(i);
		if (item == node_ptr->node.end())
			node_ptr = &node_ptr->node.insert({ i, PermissionNode() }).first->second;
		else
			node_ptr = &item->second;
	}
}

void luck::permis::PermissionNode::DeletePermission(const std::string& _permis)
{
	if (node.empty())
		return;

	auto node_list = SplitPermission(_permis);

	auto node_ptr = this;
	SelfPtrVec search_path;
	for (const auto& i : node_list)
	{
		search_path.emplace_back(node_ptr);
		if (node_ptr->node.empty())
			return;
		auto item = node_ptr->node.find(i);
		if (item == node_ptr->node.end())
			return;
		node_ptr = &item->second;
	}
	//	delete do
	auto rnode = node_list.rbegin();
	auto rfind = search_path.rbegin();
	(*rfind)->node.erase((*rfind)->node.find(*rnode));
	++rfind;
	++rnode;

	while (rfind != search_path.rend())
	{
		auto item = (*rfind)->node.find(*rnode);
		if (item->second.node.empty())
			(*rfind)->node.erase(item);
		else
			return;
		++rfind;
		++rnode;
	}
}

std::string luck::permis::PermissionNode::SelectPermission(const std::string& _permis)
{
	auto node_ptr = FindPermission(_permis);
	return "true";
}

luck::permis::PermissionNode::node_list_type luck::permis::PermissionNode::CatchListPermission(const std::string& _permis)
{
	node_list_type rst;

	auto node_ptr = FindPermission(_permis);
	for (const auto& i : node_ptr->node)
		rst.emplace_back(i.first);

	return rst;
}

luck::permis::PermissionNode::node_list_type luck::permis::PermissionNode::SplitPermission(const std::string& _permis)
{
	std::vector<std::string> rst;
	size_t index_pos = 0;
	while (true)
	{
		size_t next_pos = _permis.find('.', index_pos);
		if (next_pos == _permis.npos)
		{
			if (next_pos == _permis.size()) break;
			rst.emplace_back(_permis.substr(index_pos, _permis.size() - index_pos));
			break;
		}
		rst.emplace_back(_permis.substr(index_pos, next_pos - index_pos));
		index_pos = ++next_pos;
	}
	return rst;
}

luck::permis::PermissionNode* luck::permis::PermissionNode::FindPermission(const std::string& _permis)
{
	if (_permis.empty())
		return this;

	auto node_list = SplitPermission(_permis);
	PermissionNode* node_ptr = this;
	for (const auto& i : node_list)
	{
		auto item = node_ptr->node.find(i);
		if (item == node_ptr->node.end())
			return nullptr;
		node_ptr = &item->second;
	}
	return node_ptr;
}

luck::permis::PermissionGroup::PermissionGroup(const std::string& _name) : name(_name)
{
}

void luck::permis::PermissionGroup::InsertPermission(const std::string& _permis)
{
	base.InsertPermission(_permis);
}

void luck::permis::PermissionGroup::DeletePermission(const std::string& _permis)
{
	base.DeletePermission(_permis);
}

void luck::permis::PermissionGroup::InsertLink(const SelfShare& _share)
{
	links.emplace_back(_share);
}


