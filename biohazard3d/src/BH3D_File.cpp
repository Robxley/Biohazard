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

#include <algorithm>
#include <fstream>

#include "BH3D_File.hpp"
#include "BH3D_Logger.hpp"
#include "BH3D_Common.hpp"


#ifdef __ANDROID__
#include <SDL.h>  //utilisation de la fonction SDL_RWFromFile pour ouvrir un fichier
#include "SDLSmart.hpp"
#endif


namespace bh3d
{
	File::File(const File & file) : m_path(file.m_path)
	{

	}
	File::File(const std::filesystem::path& path) :
		m_path(path)
	{

	}

	bool File::ReadAndSaveIn(std::string & buffer) const
	{
#ifdef __ANDROID__

		SDL_RWops *file = SDL_RWFromFile(m_path.c_str(), "rb");
		if (file == nullptr)
		{
			BH3D_LOGGERERROR("Can't open the file : " << m_path);
			return BH3D_ERROR;
		}

		std::size_t bufferLenght = SDL_RWseek(file, 0, RW_SEEK_END);
		if(bufferLenght <= 1)
			BH3D_LOGGERERROR("File is empty : " << m_path);

		buffer.resize(bufferLenght,0);
		SDL_RWseek(file, 0, RW_SEEK_SET);
		size_t n_blocks = SDL_RWread(file, (void*)&buffer.front(), 1, bufferLenght);
		SDL_RWclose(file);
		file = nullptr;

		return BH3D_OK;

#else
		// On ouvre le fichier du code source
		std::ifstream ifs(m_path);

		// On verifie que le fichier existe
		if (!ifs.is_open())
		{
			BH3D_LOGGER_ERROR("File no exist : " << m_path);
			return BH3D_ERROR;
		}

		//save the file data in the buffer
		buffer.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

		ifs.close();
		//buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());


		if (buffer.empty())
		{
			BH3D_LOGGER_ERROR("File Empty : " << m_path);
			return BH3D_ERROR;
		}
		return BH3D_OK;
#endif
	}


	void File::WriteIn(const std::string & data, bool erase)
	{
		std::ofstream ofs;

		if (erase)
			ofs.open(m_path);
		else
			ofs.open(m_path,std::ofstream::app);

		ofs.write(data.c_str(), data.size());
		
		ofs.close();
	}

	



}