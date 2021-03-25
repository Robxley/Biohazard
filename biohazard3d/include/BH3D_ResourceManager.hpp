/*
 * Biohazard3D
 * The MIT License
 *
 * Copyright 2014 Robxley (Alexis Cailly).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <functional>
#include <filesystem>
#include <map>
#include <sstream>
#include <atomic>

#include "BH3D_Logger.hpp"
#include "BH3D_Bindgleton.hpp"

namespace bh3d
{
	template <typename T, typename ResT>
	class ResourceManager : public Bindgleton< T >
	{
		//singleton
		static auto GetDefaultName() {
			thread_local static std::atomic<unsigned int> m_default_name_id = 0;
			return std::string("ResourceManager ") + std::to_string(m_default_name_id++);
		}

	public:

		ResourceManager(const std::string & name = {}, bool bind = true) : m_name(name.empty() ? GetDefaultName() : name)
		{
			BH3D_LOGGER("ResourceManager Start (" << m_name << ")");
			if (bind)
				this->Bind();
		}
		ResourceManager(bool bind = true) : ResourceManager({}, bind) {}
	

		
		virtual ~ResourceManager()
		{
			Clear();
			BH3D_LOGGER("ResourceManager End (" << m_name << ")");
		}

	public:

		ResT Load(const std::filesystem::path & pathname, const std::string & resource_name = {});	//Load the resource from file and give a resouce name for identification. if the resource_name is empty take pathname

		ResT Load(const void * data, std::string resource_name);									//Load the resource from file and give a resouce name for identification

		ResT Add(ResT && resource, std::string resource_name);										//Add a resource to the manager


		//Number of resources
		std::size_t Size() const;

		//Get the resource list in a string
		void GetResourceList(std::string & list, const std::string & splitter = "\n") const;

		//Delete a resource by name
		bool Erase(const std::string & resource_name);
		inline bool Erase(const std::filesystem::path & pathname) { return this->Erase(pathname.generic_string()); }

		//Delete a resource by adresse
		bool Erase(ResT * resource);
		inline bool Erase(ResT & resource) { return Erase(&resource); }

		//Delete the overall resouces
		void Clear();

		//Set a resouce as default resource (if something bad happens in Load function, the default resource is retourned by the functio
		void SetDefault(const ResT & resource) {
			m_defaultResource = resource;
		}


		using SFreeResourceCallBack = std::function<void(ResT&)>;

		//Set a call back function call on each resource in clear function
		void SetFreeResourceCallBack(SFreeResourceCallBack && fct_callback) {
			m_freeResourceCallBack = fct_callback;
		};

		//Virtual function
		virtual bool LoadResourceFromFile(const std::filesystem::path & pathname, ResT & resource) = 0;
		virtual bool LoadResourceFromRaw(const void * data, ResT & resource) = 0;

		virtual ResT GetDefaultResouce() {
			return m_defaultResource;
		}

		void FreeResource(ResT & resource);

	protected:
		std::string m_name = "default";						//! Manager Name/ID
		std::map<std::string, ResT> m_mapResources;
		ResT m_defaultResource;
		SFreeResourceCallBack m_freeResourceCallBack;
	};

	//Declaration des fonctions
	//-------------------------------------------------------

	template <typename T, typename ResT>
	ResT ResourceManager<T, ResT>::Load(const std::filesystem::path & pathname, const std::string & resource_name)
	{
		if (pathname.empty())
		{
			assert(!pathname.empty() && "Empty path");
			return GetDefaultResouce();
		}

		//Add the resource to the map
		auto valid_resource_name = resource_name.empty() ?
			pathname.generic_string() :
			resource_name;

		//Check if the resource already exist
		if (auto it = m_mapResources.find(valid_resource_name); it != m_mapResources.end())
			return it->second;

		//load the resource (with a virtual fonction)
		ResT resource;
		if (!LoadResourceFromFile(pathname, resource))
		{
			BH3D_LOGGER_WARNING("Can't load the file resource : " << pathname);
			return GetDefaultResouce();
		}
		

		auto & ref = m_mapResources[valid_resource_name] = std::move(resource);

		BH3D_LOGGER("Resource ok : " << pathname << " (" << m_name << ")");

		return ref;
	}

	template <typename T, typename ResT>
	ResT ResourceManager<T, ResT>::Load(const void * data, std::string resource_name)
	{
		if (data == nullptr)
		{
			BH3D_LOGGER_ERROR("Raw Resource -> nullptr");
			return GetDefaultResouce();
		}

		//if resource_name is empty use the adresse as id
		if (resource_name.empty()) {
			std::stringstream ss;
			ss << data;
			resource_name = ss.str();
		}

		//Check if the source doesn't already exist
		if (auto it = m_mapResources.find(resource_name); it != m_mapResources.end())
			return it->second;

		//Load the source
		ResT resource;
		if (!LoadResourceFromRaw(data, resource))
		{
			BH3D_LOGGER_WARNING("Can't load the raw resource : " << resource_name);
			return GetDefaultResouce();
		}

		//Add the source to the map
		auto & ref = m_mapResources[resource_name] = std::move(resource);

		BH3D_LOGGER("Raw Resource ok : " << resource_name << " (" << m_name << ")");

		return ref;

	}

	template <typename T, typename ResT>
	ResT ResourceManager<T, ResT>::Add(ResT && resource, std::string resource_name)
	{
		//if resource_name is empty use the adresse as id
		if (resource_name.empty()) {
			std::stringstream ss;
			ss << (void*)(&resource);
			resource_name = ss.str();
		}

#ifdef _DEBUG
		//Check if the source doesn't already exist
		if (auto it = m_mapResources.find(resource_name); it != m_mapResources.end()) {
			BH3D_LOGGER_WARNING("A resource with the same name already exists: " << resource_name);
		}
#endif

		//Add the source to the map
		auto & ref = m_mapResources[resource_name] = std::move(resource);

		BH3D_LOGGER("Resource Add : " << resource_name << " (" << m_name << ")");

		return ref;
	}



	template <typename T, typename ResT>
	inline std::size_t ResourceManager<T, ResT>::Size() const
	{
		return m_mapResources.size();
	}

	template <typename T, typename ResT>
	void ResourceManager<T, ResT>::GetResourceList(std::string & list, const std::string & splitter) const
	{
		std::size_t fullsize = 0;
		for (auto & res : m_mapResources) {
			fullsize += res.first.size();
		}
		fullsize += splitter.size() * m_mapResources.size();

		list.reserve(fullsize);
		for (auto & res : m_mapResources) {
			list += splitter + res.first;
		}
	}

	template <typename T, typename ResT>
	bool ResourceManager<T, ResT>::Erase(const std::string & resource_name)
	{

		if (auto it = m_mapResources.find(resource_name); it != m_mapResources.end())
		{
			FreeResource(it->second);
			m_mapResources.erase(it);
			BH3D_LOGGER("Resource deleted : " << resource_name);
			return true;
		}

		BH3D_LOGGER_WARNING("Can't delete the unknown resource : " << resource_name);
		return false;
	}

	template <typename T, typename ResT>
	bool ResourceManager<T, ResT>::Erase(ResT * resource)
	{

		for (auto & res : m_mapResources)
		{
			if (res.second == *resource)
			{
				std::string file = res.first;
				FreeResource(*resource);
				m_mapResources.erase(res.first);
				BH3D_LOGGER("Resource deleted : " << file);
				return true;
			}
		}

		BH3D_LOGGER_WARNING("Can't delete the unknown resource (by ptr/ref) : " << resource);

		return false;
	}

	template <typename T, typename ResT>
	void ResourceManager<T, ResT>::FreeResource(ResT & resource) {
		if (m_freeResourceCallBack)
			m_freeResourceCallBack(resource);
	}

	template <typename T, typename ResT>
	void ResourceManager<T, ResT>::Clear()
	{
		for (auto & res : m_mapResources)
		{
			FreeResource(res.second);
			BH3D_LOGGER("Resource deleted : " << res.first);
		}
		m_mapResources.clear();
	}

}

