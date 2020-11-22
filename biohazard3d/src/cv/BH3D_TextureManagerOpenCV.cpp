
#include "BH3D_TextureManagerOpenCV.hpp"


namespace bh3d
{

	std::tuple<cv::Mat, GLenum> FormatedMatForGL(const cv::Mat & img)
	{
		cv::Mat formated_img;
		if (img.depth() != CV_8U) {
			cv::normalize(img, formated_img, 0, 255, cv::NORM_MINMAX, CV_8UC(img.channels()));
		}
		else if (!formated_img.isContinuous())
		{
			img.copyTo(formated_img);
		}

		GLenum format = 0;
		switch (img.channels())
		{
		case 1: format = GL_RED;		 break;
		case 2:	format = GL_RG;			 break;
		case 3:	format = GL_BGR;		 break;
		case 4:	format = GL_BGRA;		 break;
		default:
			assert(0 && "unsported case");
			break;
		}

		assert(formated_img.isContinuous());

		return std::tuple(formated_img, format);
	}

	MatTextureRGBAInfos& MatTextureRGBAInfos::operator<<(const cv::Mat& mat)
	{
		auto [img, format] = FormatedMatForGL(mat);
		m_img = img;

		this->m_width = m_img.cols;
		this->m_height = m_img.rows;
		this->m_format = format;
		this->m_type = GL_UNSIGNED_BYTE;
		this->m_pixels = m_img.ptr<const void>();
		return *this;
	}

	Texture TextureManagerOpenCV::CreateTextureRGBA(const cv::Mat & img)
	{

		auto[formated_img, format] = FormatedMatForGL(img);

		auto texture = TextureManager::CreateTextureRGBA(
			img.cols,
			img.rows,
			format,
			GL_UNSIGNED_BYTE,
			formated_img.ptr<const void>()
		);

		return texture;

	}

	Texture TextureManagerOpenCV::AddTexture(const cv::Mat & img, const std::string & texture_name)
	{

		auto[formated_img, format] = FormatedMatForGL(img);

		auto & textureManager = BH3D_TextureManager();
		auto texture = textureManager.AddTextureRGBA(
			img.cols,
			img.rows,
			format,
			GL_UNSIGNED_BYTE,
			formated_img.ptr<const void>(),
			texture_name
		);

		return texture;
	}


	std::tuple<cv::Mat, Texture> TextureManagerOpenCV::LoadMatTexture(const std::filesystem::path & pathname, const std::string & resource_name) 
	{
		cv::Mat img = cv::imread(pathname.generic_string(), cv::IMREAD_UNCHANGED);
		if (img.empty()) {
			BH3D_LOGGER_WARNING("Opencv can't load the image :" << pathname);
			return {};
		}
		auto texture = Load(img, resource_name.empty() ? pathname.generic_string() : resource_name);
		return { img, texture };
	}


	std::tuple<cv::Mat, Texture> TextureManagerOpenCV::AddTexture(const std::filesystem::path pathname, const std::string & texture_name)
	{
		cv::Mat img = cv::imread(pathname.generic_string(), cv::IMREAD_UNCHANGED);
		if (img.empty()) {
			BH3D_LOGGER_WARNING("Opencv can't load the image :" << pathname);
			return {};
		}
		auto texture = AddTexture(img, texture_name.empty() ? pathname.generic_string() : texture_name);
		return { img, texture };
	}

	std::tuple<cv::Mat, Texture> TextureManagerOpenCV::LoadProcessMatTexture(const std::filesystem::path & pathname, std::function<cv::Mat(const cv::Mat&)> proc, const std::string & resource_name)
	{
		cv::Mat img;
		try
		{
			img = cv::imread(pathname.generic_string(), cv::IMREAD_UNCHANGED);
		}
		catch (...)
		{
			BH3D_LOGGER_ERROR("Can't read the image : " + pathname.generic_string());
			img = cv::Mat();
		}

		if (img.empty()) {
			BH3D_LOGGER_WARNING("Opencv can't load the image :" << pathname);
			return {};
		}
		if (proc)
			img = proc(img);
		auto texture = Load(img, resource_name.empty() ? pathname.generic_string() : resource_name);
		return { img, texture };
	}

	bool TextureManagerOpenCV::LoadResourceFromFile(const std::filesystem::path & pathname, Texture& texture) 
	{
		cv::Mat img = cv::imread(pathname.generic_string(), cv::IMREAD_UNCHANGED);
		if (img.empty()) {
			BH3D_LOGGER_WARNING("Opencv can't load the image :" << pathname);
			return false;
		}
		texture = TextureManagerOpenCV::CreateTextureRGBA(img);
		return texture.IsValid();
	}

	bool TextureManagerOpenCV::LoadResourceFromRaw(const void * input, Texture& texture) 
	{
#ifdef _DEBUG
		assert(input != nullptr);  //It's normally impossible.
#endif
		texture = TextureManagerOpenCV::CreateTextureRGBA(*((const cv::Mat*)input));
		return texture.IsValid();
	}

}