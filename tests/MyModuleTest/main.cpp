#include "BHM_ModuleProc.h"

using namespace bhd;


/// <summary>
/// Create a simple structure to describe the parameters of the opencv bilateral filter (see cv::bilateralFilter)
/// Optional: A list function can be add to the structure, and it will be called automatically by CModule<T>. See example of BilateralModule
/// </summary>
struct BilateralConfigurables
{

	/// <summary>
	/// Declaration of Configurable 
	/// </summary>

	CIntConfigurable m_iDiameter = {
		"DIAMETER",					// id key

		"Neighborhood diameter",	// info

		"Diameter of each pixel neighborhood that is used during filtering."// optional blurb (opencv doc)
		" If it is non-positive, it is computed from sigmaSpace.",

		5	// default value
	};

	CDoubleConfigurable m_dSigmaColor = {
		"SIGMA_COLOR",

		"Filter sigma in the color space",

		"A larger value of the parameter means that farther colors within the pixel"
		" neighborhood (see sigmaSpace) will be mixed together, resulting in larger areas of semi-equal color.",

		15.0
	};

	CDoubleConfigurable m_dSigmaSpace = {
		"SIGMA_SPACE",

		"Filter sigma in the coordinate space",

		"A larger value of the parameter means that farther pixels will influence each other as "
		"long as their colors are close enough (see sigmaColor ). When d>0, it specifies the neighborhood "
		"size regardless of sigmaSpace. Otherwise, d is proportional to sigmaSpace.",

		15.0
	};

	CIntConfigurable m_iBorderType = {
		"BORDER_TYPE",

		"Border mode",

		"Border mode used to extrapolate pixels outside of the image",

		(int) cv::BorderTypes::BORDER_DEFAULT
	};

	/// <summary>
	/// Create a list of configurables. Auto call by CModule during the initialization if the function exist
	/// </summary>
	/// <returns>list of configurables (as a std::vector<IConfigurable*>)std</returns>
	IModule::configurable_register_t List()
	{
		return 
		{
			&m_iDiameter,
			&m_dSigmaColor,
			&m_dSigmaSpace,
			& m_iBorderType
		};
	}

};


class BilateralModule : public CModule<BilateralConfigurables>
{
	//An another configurable.
	//Generally it is preferable declare this one also in a structure
	CIntConfigurable m_iMedianKernel =
	{
		"MEDIAN_KERNEL",											//KEY
		"Median kernel",											//info
		"Median kernel after each bilateral filter application",	//blurb
		5															//default value
	};

public:

	BilateralModule() :
		//Set the bilateral module with a new key, info, blurb.
		//The registration of configurables is automatically performed by call BilateralConfigurables::List() function
		CModule(
			"BILATERAL",									    //Key
			"Bilateral filter",									//Info
			"Applies the bilateral filter to an image."			//Blurb
		)
	{	
		
	}

	void Execute(const cv::Mat& in, cv::Mat& out) const
	{
		cv::Mat tmp = in.clone();
		for (int i = 0; i < 5; i++)
		{
			cv::bilateralFilter(
				tmp,
				out,
				m_iDiameter,
				m_dSigmaColor,
				m_dSigmaSpace,
				m_iBorderType
			);
			cv::medianBlur(out, tmp, 5);
		}
		cv::swap(tmp, out);
	}

	cv::Mat Execute(const cv::Mat& in) const
	{
		cv::Mat out;
		Execute(in, out);
		return out;
	}

};


int main(int argc, char* argv[])
{
	BilateralModule bilateral;

	std::cout << bilateral.Infos() << std::endl;

	cv::Mat in(256, 128, CV_8UC3);
	cv::randn(in, 5.0f, 10.0);
	cv::Mat out = bilateral.Execute(in);

	bilateral.ImportOrExportFile("bilateral.json");

	return 0;

}