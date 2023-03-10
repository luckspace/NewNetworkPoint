#pragma once

#include <string>
#include <vector>
#include <map>

#include <queue>
#include <condition_variable>
#include <mutex>

namespace luck::permis
{
	class PermissionNode
	{
	public:
		using node_type	= std::string;
		using node_list_type = std::vector<node_type>;
		using node_map_type = std::map<node_type, PermissionNode>;

		using SelfPtr = PermissionNode*;
		using SelfPtrVec = std::vector<SelfPtr>;

		bool VerifyPermission(const std::string& _permis);

		void InsertPermission(const std::string& _permis);
		
		void DeletePermission(const std::string& _permis);

		std::string SelectPermission(const std::string& _permis);

		node_list_type CatchListPermission(const std::string& _permis);
	private:

		node_list_type SplitPermission(const std::string& _permis);

		PermissionNode* FindPermission(const std::string& _permis);

		node_map_type node;
	};

	class PermissionGroup
	{
	public:

		using Self = PermissionGroup;

		using SelfPtr = Self*;
		using SelfShare = std::shared_ptr<Self>;

		using SelfShareVec = std::vector<SelfShare>;
		using StrMapSelf = std::map<std::string, SelfShare>;

		PermissionGroup(const std::string& _name);

		void InsertPermission(const std::string& _permis);

		void DeletePermission(const std::string& _permis);

		void InsertLink(const SelfShare& _share);
	private:
		std::string name;

		PermissionNode base;
		SelfShareVec links;
	};
}
