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

#include <map>
#include <functional>
#include <algorithm>
#include <optional>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "BH3D_GL.hpp"

#include "BH3D_Viewport.hpp"

namespace bh3d
{	

	/// <summary>
	/// Features for the projection matrix
	/// </summary>
	struct CameraProjectionInfos
	{
		//Features for the projection matrix
		float m_angle_fov = glm::radians(60.0f);	//! Fov angle in radian
		float m_zNear = 0.1f;
		float m_zFar  = 1000.0f;

		void SetAngleFov(float radian) { m_angle_fov = radian; }
		void SetZNear(float znear) { m_zNear = znear; }
		void SetZFar(float zfar) { m_zFar = zfar; }
	};

	/// <summary>
	/// Features of the camera (used to compute the modelview matrix)
	/// 
	/// Be aware:
	/// The zoom parameter is a "translation" (not a zoom using the angle of fov). 
	/// That means the camera "virtual position" (in the 3d world) is computed as : virtual position = m_target + (m_position - m_target) * m_zoom).
	/// so by default m_zoom = 1.0f, like this the camera "virtual position" is the camera position.
	/// </summary>
	struct CameraLookAtInfos
	{
		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };			//! Camera position
		glm::vec3 m_direction = { 0.0f, 0.0f, 1.0f };			//! Direction vector of the camera. Linked with the target.
		glm::vec3 m_up = { 0.0f, 1.0f, 0.0f };					//! The up of the camera
		glm::vec3 m_target = { 0.0f, 0.0f, 1.0f };				//! Target of the camera. Linked with the m_direction and m_lookAtTarget
		bool m_use_target = false;								//! If true, m_target is preferable to m_direction (for the computation of the modelview matrix)


		void SetLookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) 
		{
			m_position = position;
			m_target = target;
			m_up = up;
			m_use_target = true;
		}

		/// <summary>
		/// Set the position of the lookAt matrix
		/// </summary>
		/// <param name="position">Camera position</param>
		void SetLookAtPosition(const glm::vec3& position) { m_position = position; }

		/// <summary>
		/// Set the direction of the lookAt matrix
		/// </summary>
		/// <param name="direction">Camera direction</param>
		void SetLookAtDirection(const glm::vec3& direction) { m_direction = direction; }

		/// <summary>
		/// Set the up/top of the lookAt matrix
		/// </summary>
		/// <param name="up">Camera up/top</param>
		void SetLookAtUp(const glm::vec3& up) { m_up = up; }

		/// <summary>
		/// Set the lookAt "target". The point focuses by the camera.
		/// </summary>
		/// <param name="target"></param>
		void SetLookAtTarget(const glm::vec3& target) { m_target = target; }

		/// <summary>
		/// Use the target instead of the direction to compute the lookAt matrix
		/// </summary>
		/// <param name="flag"></param>
		void SetUseTarget(bool flag) { m_use_target = flag; }
	};

	/// <summary>
	/// Main camera matrices ( projection, modelview and transform )
	/// </summary>
	struct CameraMatrices
	{
		//Camera matrices (identity by default)
		glm::mat4 m_projection = glm::mat4(1.0f);	//! Projection matrix (camera projection in the viewport)
		glm::mat4 m_modelview = glm::mat4(1.0f);	//! Look At/Model-view matrix (camera matrix position in the 3d world)
		glm::mat4 m_transform = glm::mat4(1.0f);	//! Transform matrix (world transform matrix)

		const glm::mat4& Projection() const {
			return m_projection;
		}
		const glm::mat4& Modelview() const {
			return m_modelview;
		}
		const glm::mat4& Transform() const {
			return m_transform;
		}

		/// <summary>
		/// Multiplication of the projection, lookAt and transform matrix.
		/// </summary>
		/// <returns>Multiplication Result </returns>
		glm::mat4 ProjViewTransform() const {
			return m_projection * m_modelview * m_transform;
		}

		/// <summary>
		/// Multiplication of the projection, lookAt matrix
		/// </summary>
		/// <returns>Multiplication Result</returns>
		glm::mat4 ProjView() const {
			return m_projection * m_modelview;
		}

		/// <summary>
		/// Extract the camera position from the modelview matrix.
		/// (by supposing the model view matrix don't contain scale factors for example when it is build form a LookAt function)
		/// </summary>
		/// <returns>Modelview camera position</returns>
		glm::vec3 ExtractCameraPosition() const { return ExtractCameraPosition_NoScale(m_modelview); }
	
		/// <summary>
		/// Extract the camera position from the modelview matrix.
		/// (by supposing the model view matrix don't contain scale factors for example when it is build form a LookAt function)
		/// </summary>
		/// <param name="modelView">Modelview matrix (without any scale factor, for example build from a LookAt function)</param>
		/// <returns>Modelview camera position</returns>
		static glm::vec3 ExtractCameraPosition_NoScale(const glm::mat4& modelView)
		{
			glm::mat3 rotMat(modelView);
			glm::vec3 d(modelView[3]);
			glm::vec3 retVec = -d * rotMat;
			return retVec;
		}

		void SetProjection(const glm::mat4& projection) {
			m_projection = projection;
		}

		void SetModelview(const glm::mat4& modelview) {
			m_modelview = modelview;
		}

		void SetTransform(const glm::mat4& transform) {
			m_transform = transform;
		}

	};

	/// <summary>
	/// An abstract camera class used to manage the 3d world
	/// For more description see https://learnopengl.com/Getting-started/Camera
	/// </summary>
	class Camera : public CameraProjectionInfos, public CameraLookAtInfos, public CameraMatrices
	{
	public:

		// Camera movement adjustments
		float m_speed_movement = 15.0f;							//! The scaling quantity of movement
		float m_speed_mouse = 0.008f;							//! The mouse sensitivity
		float m_fps_elapse_time = 1.0f / 60.0f;					//! Frame per second to normalize each camera movement

		// To manipulate the position of -> CameraLookAtInfos
		float m_zoom = 1.0f;									//! Zoom value on the target (it is not a zoom using the fov angle. It's a "translation". The camera position is computed as : target + (position - target) * zoom)
		float m_zoom_speed = 1.0f;								//! Zoom speed
		float m_zoom_max = 500;									//! Max zoom value
		float m_zoom_min = 0;									//! Min zoom value
		float m_zoom_default = 1.0f;							//! Default zoom value

		// To manipulate the scaling factor of -> CameraMatrices::m_transform
		float m_scale = 1.0f;							//! World global scale factor			
		float m_scale_max = 10.0f;						//! Scale factor max value
		float m_scale_min = 0.25f;						//! Scale factor min value
		float m_scale_speed = 0.1f;						//! Scale speed factor value
		float m_scale_default = 1.0f;					//! Default Scale value
			
		/// <summary>
		/// Zoom clamp in the min-max ranges
		/// </summary>
		void ZoomClamp() {
			this->m_zoom = std::clamp(this->m_zoom, this->m_zoom_min, this-> m_zoom_max);
		}
		
		/// <summary>
		/// Update the modelview matrix
		/// </summary>
		inline void LookAt() {
			if(m_use_target)
				m_direction = glm::normalize(m_target - m_position);
			else  
				m_target = m_position + m_direction; 
			LookAt(m_position, m_target, m_up, m_zoom);
		}

		/// <summary>
		/// LookAt function to pose the camera in the world 3D
		/// </summary>
		/// <param name="position">Camera position in world</param>
		/// <param name="target">The watched point by the camera</param>
		/// <param name="up">The direction of the top of the world</param>
		void LookAt(const glm::vec3 & position, const glm::vec3 & target, const glm::vec3 & up);

		/// <summary>
		/// LookAt function with zoom
		/// </summary>
		/// <param name="position">Camera position in world</param>
		/// <param name="target">The watched point by the camera (Center of the world) </param>
		/// <param name="up">The direction of the top of the world</param>
		/// <param name="zoom">Zoom on the center</param>
		inline void LookAt(const glm::vec3 & position, const glm::vec3 & target, const glm::vec3 & up, float zoom) {
			glm::vec3 eye_zoom = (zoom != 1.0f) ? target + (position - target) * zoom : position;
			LookAt(eye_zoom, target, up);
		}

		/// <summary>
		/// Look at a specific point.
		/// Don't forget to call LookAt function to recompute the camera matrix (or wait a update of a CameraEngine)
		/// </summary>
		/// <param name="target">point to target</param>
		inline void LookAtTarget(const glm::vec3 & target) {
			m_target = target;
			m_direction = glm::normalize(target - m_position);
		}

		/// <summary>
		/// Look at a specific point.
		/// Set updateNow to true, or don't forget to call LookAt function to recompute the camera matrix (or wait a update of a CameraEngine)
		/// </summary>
		/// <param name="target">point to target</param>
		/// <param name="updateMatrix">If true update the lookAt matrix</param>
		inline void LookAtTarget(const glm::vec3 & target, bool updateMatrix) {
			LookAtTarget(target);
			if(updateMatrix)
				LookAt();
		}

		/// <summary>
		/// Keep to Look at a specific point.
		/// Don't forget to call LookAt function to recompute the camera matrix (or wait a update of a CameraEngine)
		/// </summary>
		/// <param name="target">point to target</param>
		inline void KeepToLookAtTarget(const glm::vec3& target) {
			m_use_target = true;
			LookAtTarget(target);
		}

		/// <summary>
		/// Keep to Look at a specific point.
		/// Set updateMatrix to true or don't forget to call LookAt function to recompute the camera matrix (or wait a update of a CameraEngine)
		/// </summary>
		/// <param name="target">point to target</param>
		/// <param name="updateMatrix">If true update the lookAt matrix</param>
		inline void KeepToLookAtTarget(const glm::vec3& target, bool updateMatrix) {
			m_use_target = true;
			LookAtTarget(target, updateMatrix);
		}

		/// <summary>
		/// Reset the zoom value with default zoom value
		/// And Reset the camera transform matrix with the identity matrix
		/// Don't forget to call LookAt() function to recompute the camera matrix (or wait a update of a CameraEngine)
		/// </summary>
		void Identity() {
			m_zoom = m_zoom_default;
			m_scale = m_scale_default;
			m_transform = glm::mat4(1.0f);
		}

		/// <summary>
		/// Reset the zoom value with default zoom value
		/// And Reset the camera transform matrix with the identity matrix
		/// Then call the lookAt() function to update the camera matrix if the flag updateMatrix is true
		/// </summary>
		/// <param name="updateMatrix">If true, call LookAt() function to update the camera matrix</param>
		void Identity(bool updateMatrix) {
			Identity();
			if (updateMatrix)
				LookAt();
		}

		/// <summary>
		/// Compute a lookAt matrix from a modelview/transform matrix (transform matrix have only be a combination of translation and rotation, without scaling)
		/// </summary>
		/// <param name="pose">transformation matrix without scaling / shrinking </param>
		/// <param name="direction"></param>
		/// <param name="up"></param>
		void SetLookAtFromPose(const glm::mat4& pose, const glm::vec3& direction, const glm::vec3& up)
		{
			std::tie(m_position, m_direction, m_up) = ExtractLookAtInfos(pose, direction, up);
			m_use_target = false;
			LookAt();
		}

		using LookAtInfos = std::tuple<glm::vec3, glm::vec3, glm::vec3>; //! As position, target, and up 

		/// <summary>
		/// Apply the LookAt parameters to a transform matrix
		/// </summary>
		/// <param name="pose">transform matrix</param>
		/// <param name="direction">Direction of the lookAt camera</param>
		/// <param name="up">Up of the lookAt camera</param>
		/// <returns>Look at infos tuple</returns>
		static LookAtInfos ExtractLookAtInfos(const glm::mat4& pose, const glm::vec3& direction, const glm::vec3& up);

	protected:

		inline glm::vec3 DirectionRight() { return glm::normalize(glm::cross(m_up, m_direction)); }

		inline auto Velocity()			{ return m_speed_movement * m_fps_elapse_time; }
		inline void MoveRight()			{ m_position -= DirectionRight() * Velocity(); }
		inline void MoveLeft()			{ m_position += DirectionRight() * Velocity(); }
		inline void MoveForward()		{ m_position += (m_direction) * Velocity(); }
		inline void MoveBackWard()		{ m_position -= (m_direction) * Velocity(); }
		inline void MoveUp()			{ m_position += (m_direction) * Velocity(); }
		inline void MoveDown()			{ m_position -= (m_direction) * Velocity(); }
	};

	enum CameraMod { CameraMod_LookAround,  CameraMod_FreeFlight, CameraMod_TrajectoryFlight, CameraMod_COUNT };


	class Mouse;		//See BH3D_Event.hpp
	class CameraEngine : public Viewport, public Camera
	{
	public:
		int m_cameraMod = CameraMod_LookAround;
		/// <summary>
		/// Resize function.
		/// Generally to call when the viewport (or the screen are resize)
		///   Camera definition :
		/// 		 / | 
		///			|  |			|
		///	     	|  |			|	
		///			 \ |
		///		----------------------------				
		///	   near |  | far	   Fov
		/// </summary>
		/// <param name="w">New width for the viewport (or sensor width).</param>
		/// <param name="h">New height for the viewport (or sensor height).</param>
		/// <param name="angle_fov">Field of view of the camera. Builds a perspective projection matrix based on a field of view. </param>
		/// <param name="near">The closest plane from which objects are drawn on the screen.</param>
		/// <param name="far">The farthest plane from which objects are no longer drawing on the screen.</param>
		void Resize(unsigned int w, unsigned int h, float angle_fov, float zNear, float zFar) {
			assert(w > 0 && h > 0 && angle_fov > 0);
			m_width = w; m_height = h;
			m_angle_fov = glm::radians(angle_fov);
			m_zNear = zNear;
			m_zFar = zFar;
			Resize();
		}

		/// <summary>
		/// Resize function.
		/// </summary>
		/// <param name="w">New width for the viewport</param>
		/// <param name="h">New height for the viewport</param>
		void Resize(unsigned int w, unsigned int h) {
			assert(w > 0 && h > 0);
			m_width = w; m_height = h;
			Resize();
		}

		/// <summary>
		///  Resize function.
		/// </summary>
		void Resize() {
			assert(m_width > 0 && m_height > 0);
			if(m_width > 0 && m_height > 0)
				m_projection = glm::perspectiveFov(m_angle_fov, (float)m_width, (float)m_height, m_zNear, m_zFar);
		}

		//Update the camera matrices
		void UpdateMatrices()
		{
			ZoomClamp();
			Resize();
			LookAt();
		}

		void Update()
		{
			UpdateMatrices();
		}

		/// <summary>
		/// Update the movement of the camera according to the movement of the mouse and the selected mod
		/// </summary>
		/// <param name="m_mouse">Mouse inputs</param>
		virtual void Update(const Mouse &mouse)
		{
			assert(m_cameraMod != CameraMod_TrajectoryFlight && "Use a CameraTrajectory object with this mod");
			switch (m_cameraMod)
			{
			case CameraMod_LookAround:
				LookAround(mouse);
				return;
			default:
				FreeFlight(mouse);
				return;
			}
		}

	protected:
		/// <summary>
		/// Look around the target point according to the movement of the mouse
		/// </summary>
		/// <param name="m_mouse">Mouse inputs</param>
		void LookAround(const Mouse &mouse);

		/// <summary>
		/// Free movement of the camera according to the movement of the mouse
		/// </summary>
		/// <param name="m_mouse">Mouse inputs</param>
		void FreeFlight(const Mouse &mouse);

	};

	/// <summary>
	/// The movements of the camera are defined according to a trajectory.
	/// </summary>
	class CameraTrajectory : public CameraEngine
	{
	public:

		CameraTrajectory() {
			m_cameraMod = CameraMod_FreeFlight;
		}

		int m_key = 0;																			//! Key/id position in the trajectory.
		std::variant<int, std::function<int()>> m_maxKey = std::numeric_limits<int>::max();		//! maximum value for the look at "key" (not included lie std::vector::size or any other container size )
		bool m_loop = false;																	//! Perform a loop when the max/min "key" value is rushed, else the value is clamp to the [0, m_maxLookAtID]
		
		using LookAtParams = std::optional<std::tuple<glm::vec3, glm::vec3, glm::vec3>>;
		std::function<LookAtParams(unsigned int i)> m_lookAtLambda = {};						//! User lambda called to get LookAt parameter from the current key;

		/// <summary>
		/// Set the key. If necessary, the key value is clamp in valid range [0, m_maxKey] and following a loop option.
		/// </summary>
		/// <param name="key">key value</param>
		void SetKey(int key) {
			m_key = key;
		}

		/// <summary>
		/// Refresh the camera status
		/// </summary>
		inline void Refresh() {
			ExtractLookAtParams();
			LookAt();
		}

		/// <summary>
		/// Return the maximum key value. (The returned value it's not included, like std::vector::size()).
		/// </summary>
		/// <returns>Max value. (Not included, like std::vector::size() or any std container) </returns>
		inline int MaxKeyValue() {
			return std::visit([](auto && int_or_lambda)->int {
				using T = std::decay_t<decltype(int_or_lambda)>;
				if constexpr (std::is_same_v<T, int>)
					return int_or_lambda;
				else
					return int_or_lambda();
			}, m_maxKey);
		}

		void Update(const Mouse &m_mouse) override
		{
			switch (m_cameraMod)
			{
			case CameraMod_LookAround:
				LookAround(m_mouse);
				return;
			case CameraMod_FreeFlight:
				FreeFlight(m_mouse);
				return;
			default:
				TrajectoryFlight(m_mouse);
				return;
			}
		}


	private:

		void TrajectoryFlight(const Mouse &m_mouse);

		/// <summary>
		/// Extract the lookAt parameters from the LookAt lambda function
		/// </summary>
		inline void ExtractLookAtParams() {
			m_key = interval_int(m_key, 0, MaxKeyValue(), m_loop); // Clamp the value of m_key in the range [0, MaxKeyValue()[
			LookAtParams new_params = m_lookAtLambda(m_key);
			if (new_params.has_value())
			{
				auto&[new_pos, new_dir, new_up] = new_params.value();
				m_position = new_pos;
				m_direction = new_dir;
				m_up = new_up;
			}
		}
	
	};


}
