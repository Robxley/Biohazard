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

namespace bh3d
{
	/*!
	\author 	Robxley (A.CAILLY)
	\version	0.1
	\date		08/12/2014
	\~english
	\brief		Definition of a triangular face.
	\remark		It's an unsigned int[3] = {0,0,0} array with some routines
	\~french
	\brief		Definition d'une face triangulaire
	\remark		Un simple tableau unsigned int[3] = {0,0,0} avec quelques routines.
	*/
	class Face
	{	
	public:
		unsigned int id[3] = { 0,0,0 };

		/**
		*\~english
		*\brief		Puts all indices to 0.
		*\~french
		*\brief		Met tous les indices � 0.
		*/
		inline void Reset()
		{
			id[0] = 0;
			id[1] = 0;
			id[2] = 0;
		}

		/**
		*\~english
		*\brief		Checks the validity indices
		*\return	true if all indices are differents and greaters than or equal to 0 (>=0).
		*\~french
		*\brief		true si tous les indices sont diff�rents et sup�rieurs ou �gale � 0 (>=0).
		*/
		inline bool IsValid()
		{
			return (((id[0] != id[1]) && (id[0] != id[2]) && (id[1] != id[2])));
		}
		/**
		*\~english
		*\brief		setup fonction
		*\param[in]	first indice.
		*\param[in]	2nd indice.
		*\param[in]	3th indice.
		*\~french
		*\brief		Assignement des indices
		*\param[in]	1er indice.
		*\param[in]	2nd indice.
		*\param[in]	3eme indice.
		*/
		inline void set(unsigned int id0 = 0, unsigned int id1 = 0, unsigned int id2 = 0)
		{
			id[0] = id0; 
			id[1] = id1; 
			id[2] = id2;
		}
		/**
		*\~english
		*\brief		setup fonction
		*\param[in]	Pointer to an unsigned int array of size 3.
		*\~french
		*\brief		Assignement des indices
		*\param[in]	Pointeur sur un tableau de taille 3 entiers non signed.
		*/
		inline void set(const unsigned int* _id)
		{
			id[0] = _id[0]; 
			id[1] = _id[1]; 
			id[2] = _id[2];
		}

		inline unsigned int & operator[](unsigned int i) {
			assert(i < 3);
			return id[i];
		}
	};
}

