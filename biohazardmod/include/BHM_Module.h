#pragma once

#include "BHM_UtilTraits.h"
#include "BHM_Configurable.h"

#include <variant>

namespace bhd
{
	//module default types (for quick recompilation needs)
	namespace module_types
	{
		using key_t = std::string;      //key as module id
		using string_t = std::string;   //From c++17, use u8"blah blah" for utf8 encoding
	}

	/// <summary>
	/// Module interface class.
	/// The interface class is defined to manipulated (mainly to import/export in file) a list of configurables and submodules.
	/// 
	/// Module is defined by :
	///		- a Key as identification label
	///		- a Info as string short description
	///		- a Blurb as string description
	///		- an optional alias to distinguish instances of the same module.
	/// 
	///		- a list of configurable parameters (as pointers) linked to the module for its configuration.
	///		- a list of sub-modules used by this module
	/// </summary>
	class IModule
	{

	public:
		using key_t = typename module_types::key_t;
		using string_t = typename module_types::string_t;
		using configurable_register_t = std::vector<IConfigurable*>;
		using submodule_t = object_wrapper<IModule>;
		using module_register_t = std::vector<submodule_t>;

	public:

		key_t m_sKey;                   //! Configurable Key
		string_t m_sInfo;               //! Info/description
		string_t m_sBlurb;              //! Description
		std::optional<key_t> m_sAlias;  //! Optional named module.

		configurable_register_t m_vConfigurables;   //! List of configurable pointers
		module_register_t m_vpSubModules;           //! List of submodules (as a object, pointer or ref)

		IModule(const IModule&) = default;
		IModule(IModule&&) = default;
		IModule() = default;

		IModule& operator=(const IModule&) = default;
		IModule& operator=(IModule&&) = default;

		virtual ~IModule() {};

		/// <summary>
		/// Configurable class
		/// </summary>
		/// <param name="key">KEY for identification</param>
		/// <param name="info">Description of the module </param>
		/// <param name="blurb">Blurb of the module </param>
		/// <param name="alias">Alias of the configurable </param>
		IModule(const key_t& key, const string_t& info = {}, const string_t& blurb = {}, const key_t& alias = {}) :
			m_sKey(key), 
			m_sInfo(info), 
			m_sBlurb(blurb), 
			m_sAlias(alias.empty() ? std::nullopt : std::optional<key_t>{alias})
		{		}

		template<class TConfigurables, class = std::enable_if_t<std::is_constructible_v<decltype(m_vConfigurables), TConfigurables>> > //any thing convertible to configurable_register_t
		IModule(TConfigurables&& configs, const key_t& key, const string_t& info = {}, const string_t& blurb = {}, const key_t& alias = {}) :
			m_sKey(key),
			m_sInfo(info),
			m_sBlurb(blurb),
			m_sAlias(alias.empty() ? std::nullopt : std::optional<key_t>{ alias }),
			m_vConfigurables(std::forward<TConfigurables>(configs))
		{		}

		//! Return the module key
		const key_t& GetKey()	const noexcept { return m_sKey; }
		
		//! Return the module description
		const string_t& GetInfo()	const noexcept { return m_sInfo; }
		
		//! Return the module blur
		const string_t& GetBlurb()	const noexcept { return m_sBlurb; }

		//! Return the module alias. If no alias is defined, return the module key
		const key_t& GetAlias() const { 
			assert(m_sAlias.has_value() || !GetKey().empty() && "Have to be a key or alias");
			return (m_sAlias.has_value() ? m_sAlias.value() : GetKey()); 
		}

		//! Return the type index of the module
		std::type_index TypeIndex() const { return std::type_index(typeid(*this)); }


		/// <summary>
		/// Copy all configurables, all submodules inside a other same module.
		/// Do nothing if the module key don't match.
		/// </summary>
		/// <param name="module">Destination module</param>
		void CopyTo(IModule& imodule) const
		{
			if (imodule.GetKey() != imodule.GetKey())
				return;
			//Configurables
			{
				assert(m_vConfigurables.size() == imodule.m_vConfigurables.size());
				for (std::size_t i = 0; i < m_vConfigurables.size(); i++)
				{
					assert(m_vConfigurables[i]->GetKey() == imodule.m_vConfigurables[i]->GetKey());
					assert(m_vConfigurables[i]->TypeIndex() == imodule.m_vConfigurables[i]->TypeIndex());
					m_vConfigurables[i]->CopyTo(*imodule.m_vConfigurables[i]);
				}
			}

			//Recursive on submodule
			if (!m_vpSubModules.empty())
			{
				assert(m_vpSubModules.size() == imodule.m_vpSubModules.size());
				for (std::size_t i = 0; i < m_vpSubModules.size(); i++)
				{
					assert(m_vpSubModules[i]->GetKey() == imodule.m_vpSubModules[i]->GetKey());
					assert(m_vpSubModules[i]->TypeIndex() == imodule.m_vpSubModules[i]->TypeIndex());
					m_vpSubModules[i]->CopyTo(*imodule.m_vpSubModules[i]);
				}
			}
		};

		/// <summary>
		/// clone the module. Create a new instance of the module and copy the state of its configurables.
		/// </summary>
		/// <returns>Return a copy of the module</returns>
		IModule Clone() const
		{
			IModule clone;
			CopyTo(clone);
			return clone;
		}


		/// <summary>
		/// Export the configurables/submodule status of the module into a file. If the file can't be exported, return a error.
		/// </summary>
		/// <param name="file_path">File path</param>
		/// <returns>Empty if not error, else message error</returns>
		std::string ExportFile(const std::filesystem::path & file_path) const noexcept
		{ 
			try
			{
				cv::FileStorage fs(file_path.generic_string(), cv::FileStorage::WRITE);
				if (!fs.isOpened())
					return { "FileStorage can't be opened" };
				write(fs);
			}
			catch (std::exception& e) { return e.what(); }
			catch (...) { return "unknown exception"; }
			return {};
		}

		/// <summary>
		/// Export the configurables/submodule status of the module into a file. If the file can't be exported, return a error.
		/// </summary>
		/// <param name="directory">Output directory</param>
		/// <param name="filename">Module filename</param>
		/// <returns>Empty if not error, else message error</returns>
		std::string ExportFile(const std::filesystem::path& directory, const std::filesystem::path& filename)
		{
			assert(std::filesystem::exists(directory));
			assert(filename.has_extension());
			auto file = directory / filename;
			return ExportFile(file);
		}

		/// <summary>
		/// Export the configurables/submodule status of the module into a string.
		/// </summary>
		/// <param name="format">Export format. (see cv::FileStorage::Mode)</param>
		/// <returns>Empty if not error, else message error</returns>
		std::string Infos(int format = cv::FileStorage::FORMAT_JSON)
		{
			try
			{
				cv::FileStorage fs("foo", cv::FileStorage::WRITE | cv::FileStorage::MEMORY | format);
				if (!fs.isOpened())
					return { "FileStorage can't be opened" };
				write(fs);
				return fs.releaseAndGetString();
			}
			catch (std::exception& e) { return e.what(); }
			catch (...) { return "unknown exception"; }
			return {};
		}

		/// <summary>
		/// Import configurable/submodule values from a file
		/// </summary>
		/// <param name="file_path">File path</param>
		/// <returns>Empty if not error, else message error</returns>
		std::string ImportFile(const std::filesystem::path & file_path) noexcept
		{ 
			try
			{
				cv::FileStorage fs(file_path.generic_string(), cv::FileStorage::READ);
				if (!fs.isOpened())
					return { "FileStorage can't be opened" };
				read(fs[GetAlias()]);
			}
			catch(std::exception & e) {	return e.what(); }
			catch (...) { return "unknown exception"; }
			return {};
		}

		/// <summary>
		/// Try to import configurable/submodule values from a file
		/// If fail, the file is created as template
		/// </summary>
		/// <param name="file_path">File path</param>
		/// <returns>Return a std::pair (uchar, std::string). int value as 0: import or export fail, 1: import, 2: export object. If not empty, the string value is the error of the import/export</returns>
		auto ImportOrExportFile(const std::filesystem::path & file_path) noexcept
		{
			std::pair<uchar, std::string> stats = { 1, {} };
			if (stats.second = ImportFile(file_path); !stats.second.empty())
			{	
				//Import fail, try to export file
				if (auto export_error = ExportFile(file_path); export_error.empty())
					stats.first = 2;								//OK
				else 
					stats = { 0, stats.second + export_error };    //Import & export failure message
			}
			return stats;
		}

		/// <summary>
		/// Register/add a single configurable inside the module
		/// </summary>
		/// <param name="configurable">Configurable reference</param>
		void RegisterConfigurable(IConfigurable & configurable) {
			m_vConfigurables.emplace_back(&configurable);
		}

		/// <summary>
		/// Register a list of configurables inside the module configurable list.
		/// Ex: 
		/// RegisterConfigurables(my_double, my_int, my_float);
		/// </summary>
		/// <typeparam name="...Args">Any configurable types</typeparam>
		/// <param name="...configurable_references">Lists of configurables</param>
		template<typename ... Args>
		void RegisterConfigurables(Args&& ...configurable_references)
		{
			if constexpr (sizeof...(Args) > 0)
			{
				m_vConfigurables.reserve(std::size(m_vConfigurables) + sizeof...(Args));
				(RegisterConfigurable(std::forward<Args>(configurable_references)), ...);
			}
		}

		/// <summary>
		/// Register a list of configurables inside the module
		/// </summary>
		/// <param name="list">A list of configurable</param>
		void RegisterConfigurables(const std::initializer_list<IConfigurable*>& list)
		{
			m_vConfigurables.reserve(std::size(m_vConfigurables) + std::size(list));
			m_vConfigurables.insert(std::end(m_vConfigurables), std::begin(list), std::end(list));
		}

		/// <summary>
		/// Register a list of configurables inside the module (from a container like std::vector)
		/// </summary>
		/// <param name="list">container</param>
		template<class T>
		auto RegisterConfigurables(T&& container) -> decltype(std::size(std::declval<T>()), std::begin(std::declval<T>()), std::end(std::declval<T>()), void())
		{
			m_vConfigurables.reserve(m_vConfigurables.size() + std::size(container));
			m_vConfigurables.insert(std::begin(m_vConfigurables), std::begin(container), std::end(container));
		}

		void RegisterSubModule(IModule& submodule) {
			m_vpSubModules.emplace_back(&submodule);
		}

		void RegisterSubModule(IModule&& submodule) {
			m_vpSubModules.emplace_back(std::move(submodule));
		}

		void RegisterSubModule(IModule & submodule, const key_t & alias) {
			submodule.m_sAlias = alias;
			m_vpSubModules.emplace_back(&submodule);
		}

		template <typename ...Args>
		static auto MakeConfigurableList(Args&... configurables)
		{
			static_assert((std::is_base_of_v<IConfigurable, Args> && ...));
			{
				return configurable_register_t{ (static_cast<IConfigurable*>(&configurables), ...) };
			}
		}

		template<typename ...Args>
		void RegisterAsSubModule(configurable_register_t&& vConfigurables, Args&&... module_args) {
			RegisterSubModule(
				MakeAsSubModule(std::move(vConfigurables), std::forward<Args>(module_args)...)
			);
		}

		template<typename ...Args>
		void RegisterAsSubModule(const configurable_register_t&& vConfigurables, Args&&... module_args) {
			RegisterSubModule(
				MakeAsSubModule(vConfigurables, std::forward<Args>(module_args)...)
			);
		}

		template<typename ...Args>
		static IModule MakeAsSubModule(configurable_register_t && vConfigurables, Args&&... module_args) {
			return IModule(std::move(vConfigurables), std::forward<Args>(module_args)...);
		}

		template<typename ...Args>
		static IModule MakeAsSubModule(const configurable_register_t& vConfigurables, Args&&... module_args) {
			return IModule(vConfigurables, std::forward<Args>(module_args)...);
		}

		void Reset() {
			Reset(*this);
		}

private:
	
		void Reset(IModule & module)
		{
			for (auto * config : module.m_vConfigurables)
				config->Reset();

			for (auto & submodule : module.m_vpSubModules)
				Reset(submodule.value());
		}

		void write(cv::FileStorage& fs) const //Write serialization for this class
		{
			fs << GetAlias() << "{";
			write_configurables(fs);
			write_submodules(fs);
			fs << "}";
		}

		void read(const cv::FileNode& node)  //Read serialization for this class
		{
			read_configurables(node);
			read_submodules(node);
		}


		void write_configurables(cv::FileStorage& fs) const
		{
			if (m_vConfigurables.empty())
				return;
			//fs << "Configurables" << "{";
			for (auto & config : m_vConfigurables)
				fs << *config;
			//fs << "}";
		}

		void write_submodules(cv::FileStorage& fs) const
		{
			if (m_vpSubModules.empty())
				return;
			//fs << "Modules" << "{";
			for (auto & submodule : m_vpSubModules) 
			{
				submodule->write(fs);
			}
			//fs << "}";
		}

		void read_configurables(const cv::FileNode& fs)
		{
			if (m_vConfigurables.empty())
				return;
			for (auto & config : m_vConfigurables) {
				fs >> *config;
			}
		}

		void read_submodules(const cv::FileNode& fs)
		{
			if (m_vpSubModules.empty())
				return;
	
			for (auto & submodule : m_vpSubModules)
				submodule->read(fs[submodule->GetAlias()]);
		}

	};

	template <class TConfigs, class = void>
	struct CModule : public TConfigs, public IModule
	{
		template<typename ...Args>
		CModule(Args&&... args) :
			TConfigs(),
			IModule(std::forward<Args>(args)...)
		{		}

	};

	template <class TConfigs>
	struct CModule<TConfigs, decltype(std::declval<TConfigs>().ConfigurableList(), void(0))> : public TConfigs, public IModule
	{
		template<typename ...Args>
		CModule(Args&&... args) :
			TConfigs(),
			IModule(TConfigs::ConfigurableList(), std::forward<Args>(args)...)
		{		}
	};

}
