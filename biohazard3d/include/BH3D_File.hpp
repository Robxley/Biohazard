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
#ifndef _BH3D_FILE_H_
#define _BH3D_FILE_H_

#include <filesystem>

namespace bh3d
{	

	class File
	{
	public:

		//constructeurs
		File(const File & file);
		File(const std::filesystem::path & path = "");
		File(const char * path) : File(std::filesystem::path(path)) {};

		//quelque opérateur utile
		inline operator std::string() const;
		inline operator const std::filesystem::path&() const;
	
		//File(const File & file);
		virtual bool ReadAndSaveIn(std::string & buffer) const;

		//écris dans le fichier, supprime le contenu du fichier si existant
		virtual void WriteIn(const std::string & data, bool erase = true);


		//accesseur
		inline void set(const std::filesystem::path & path);

		//----------------------------------------------------------
		// Indique si le fichier existe ou non
		//----------------------------------------------------------
		inline bool Exists() const;

		//----------------------------------------------------------
		// Renvoie le nom du fichier sans son chemin
		// ex : porte.jpg
		//----------------------------------------------------------
		inline std::string Filename() const;

		//----------------------------------------------------------
		// Renvoie le nom du fichier sans extension ni chemin
		// ex : porte
		//----------------------------------------------------------
		inline std::string ShortFilename() const;

		//----------------------------------------------------------
		// Renvoie l'extension du fichier
		// ex : jpg
		//----------------------------------------------------------
		inline std::string Extension() const;

		//----------------------------------------------------------
		// Renvoie l'extension du fichier
		// ex : data/maison
		//----------------------------------------------------------
		inline std::string Dir() const;
	private:

		//----------------------------------------------------------
		// Données membres
		//----------------------------------------------------------
		std::filesystem::path m_path; // Chemin complet du fichier
	};


	//Inline fonctions
	//------------------------------------------------

	File::operator std::string() const
	{
		return m_path.generic_string();
	}
	File::operator const std::filesystem::path&() const
	{
		return m_path;
	}

	//----------------------------------------------------------
	// Constructeur à partir d'un std::string
	//----------------------------------------------------------
	void File::set(const std::filesystem::path& name) {
		m_path = name;
	}

	bool File::Exists() const {
		return std::filesystem::exists(m_path);
	}

	std::string File::Filename() const {
		return m_path.filename().generic_string();
	}

	//----------------------------------------------------------
	// Renvoie le nom du fichier sans extension ni chemin
	// ex : porte
	//----------------------------------------------------------
	std::string File::ShortFilename() const {
		return m_path.filename().replace_extension().generic_string();
	}

	//----------------------------------------------------------
	// Renvoie l'extension du fichier
	// ex : jpg
	//----------------------------------------------------------
	std::string File::Extension() const {
		return m_path.filename().extension().generic_string();
	}

	//----------------------------------------------------------
	// Renvoie l'extension du fichier
	// ex : data/maison
	//----------------------------------------------------------
	std::string File::Dir() const {
		return m_path.filename().remove_filename().generic_string();
	}


}
#endif //
