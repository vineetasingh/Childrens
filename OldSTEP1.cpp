#include "opencv2/opencv.hpp"
#include <fstream>
#define MIN(a,b) ((a) < (b) ? (a) : (b))




using namespace cv;
using namespace std;
ofstream myfile;




const int somathreshold = 1500;
const int bluecontourthreshold = 1500;
const int houghthresh = 275;
const float tune = 0.1;
RNG rng;




void arounddendrite(Mat imm, vector<RotatedRect> rotRect, ofstream &myfile, int CountLW, int CountMW, int CountHW)
{




	int LWlcount = 0, LWmcount = 0, LWhcount = 0, MWlcount = 0, MWmcount = 0, MWhcount = 0, HWlcount = 0, HWmcount = 0, HWhcount = 0;
	int totLWlcount = 0, totLWmcount = 0, totLWhcount = 0, totMWlcount = 0, totMWmcount = 0, totMWhcount = 0, totHWlcount = 0, totHWmcount = 0, totHWhcount = 0;
	int avgLWlcount = 0, avgLWmcount = 0, avgLWhcount = 0, avgMWlcount = 0, avgMWmcount = 0, avgMWhcount = 0, avgHWlcount = 0, avgHWmcount = 0, avgHWhcount = 0;




	Mat thr_high_Synapse;
	cv::inRange(imm, cv::Scalar(0, 0, 110), cv::Scalar(30, 30, 255), thr_high_Synapse);//RED-HIGH INTENSITY
	dilate(thr_high_Synapse, thr_high_Synapse, Mat());
	//imshow("thr_high_Synapse", thr_high_Synapse);




	Mat Medium_Synapse_thr;
	cv::inRange(imm, cv::Scalar(0, 0, 50), cv::Scalar(10, 10, 175), Medium_Synapse_thr);//RED -MED INTENSITY
	dilate(Medium_Synapse_thr, Medium_Synapse_thr, Mat());




	Mat Low_Synapse_thr;
	cv::inRange(imm, cv::Scalar(0, 0, 20), cv::Scalar(10, 10, 38), Low_Synapse_thr);
	Low_Synapse_thr = Low_Synapse_thr - Medium_Synapse_thr - thr_high_Synapse;//RED -LOW INTENSITY
	dilate(Low_Synapse_thr, Low_Synapse_thr, Mat());




	int totlowIntSyn = countNonZero(Low_Synapse_thr);
	int totmedIntSyn = countNonZero(Medium_Synapse_thr);
	int tothigIntSyn = countNonZero(thr_high_Synapse);





	for (int i = 0; i < rotRect.size(); i++)
	{
		LWlcount = 0, LWmcount = 0, LWhcount = 0, MWlcount = 0, MWmcount = 0, MWhcount = 0, HWlcount = 0, HWmcount = 0, HWhcount = 0;
		Mat Hrotated, Mrotated, Lrotated;
		cv::Mat Hcropped, Mcropped, Lcropped;
		cv::Mat rot_mat = cv::getRotationMatrix2D(rotRect[i].center, rotRect[i].angle, 1);
		if (rotRect[i].angle < -45.)
			std::swap(rotRect[i].size.width, rotRect[i].size.height);





		warpAffine(thr_high_Synapse, Hrotated, rot_mat, thr_high_Synapse.size(), INTER_CUBIC);
		// crop the resulting image
		cv::getRectSubPix(Hrotated, rotRect[i].size, rotRect[i].center, Hcropped);





		warpAffine(Medium_Synapse_thr, Mrotated, rot_mat, Medium_Synapse_thr.size(), INTER_CUBIC);
		// crop the resulting image
		cv::getRectSubPix(Mrotated, rotRect[i].size, rotRect[i].center, Mcropped);





		warpAffine(Medium_Synapse_thr, Lrotated, rot_mat, Low_Synapse_thr.size(), INTER_CUBIC);
		// crop the resulting image
		cv::getRectSubPix(Lrotated, rotRect[i].size, rotRect[i].center, Lcropped);// low int synapse rotated rect (ROI)  image




		float wdth = MIN(rotRect[i].size.width, rotRect[i].size.height);
		if (wdth >= 1)
		{
			if (wdth <= 50) //low width dendrite
			{
				LWlcount = countNonZero(Lcropped); // no of low int syn around a Low width dendrite
				LWmcount = countNonZero(Mcropped);// no of med int syn around a Low width dendrite
				LWhcount = countNonZero(Hcropped);// no of high int syn around a Low width dendrite
				//cout << countNonZero(Lcropped) << ", " << countNonZero(Mcropped) << ",  " << countNonZero(Hcropped) << endl;
				totLWlcount += LWlcount; totLWmcount += LWmcount;  totLWhcount += LWhcount;
			}
			else if (wdth > 50 && wdth <= 100)
			{
				MWlcount = countNonZero(Lcropped);
				MWmcount = countNonZero(Mcropped);
				MWhcount = countNonZero(Hcropped);
				totMWlcount += MWlcount; totMWmcount += MWmcount;  totMWhcount += MWhcount;
			}
			else
			{
				HWlcount = countNonZero(Lcropped);
				HWmcount = countNonZero(Mcropped);
				HWhcount = countNonZero(Hcropped);
				totHWlcount += HWlcount; totHWmcount += HWmcount;  totHWhcount += HWhcount;
			}
		}


	}
	if (CountLW == 0) { avgLWlcount = 0; avgLWmcount = 0; avgLWhcount = 0; }
	else { avgLWlcount = totLWlcount / CountLW;  avgLWmcount = totLWmcount / CountLW; avgLWhcount = totLWhcount / CountLW; }
	if (CountMW == 0) { avgMWlcount = 0; avgMWmcount = 0; avgMWhcount = 0; }
	else { avgMWlcount = totMWlcount / CountMW; avgMWmcount = totMWmcount / CountMW; avgMWhcount = totMWhcount / CountMW; }
	if (CountHW == 0) { avgHWlcount = 0; avgHWmcount = 0; avgHWhcount = 0; }
	else { avgHWlcount = totHWlcount / CountHW; avgHWmcount = totHWmcount / CountHW; avgHWhcount = totHWhcount / CountHW; }
	//myfile << "," << "arounddendrite" << "," << "Avg Low Int Syn count arnd. Low width" << "," << "Avg Med Int Syn count arnd. Low width" << "," << "Avg Low Int Syn count arnd. High width" << "," << "Avg Low Int Syn count arnd. Med width" << "," << " Avg Med Int Syn count arnd. Med width" << "," << "Avg High Int Syn count arnd. Med width" << "," << "Avg Low Int Syn count arnd. Large width " << "," << "Avg Med Int Syn count arnd. Large width" << "," << "Avg High Int Syn count arnd. Large width";
	myfile << "," << "arounddendrite" << "," << avgLWlcount << "," << avgLWmcount << "," << avgLWhcount << "," << avgMWlcount << "," << avgMWmcount << "," << avgMWhcount << "," << avgHWlcount << "," << avgHWmcount << "," << avgHWhcount;
}

void dendritecalc(string imname, Mat thresh, vector<Vec4i> lines, Mat  redlow, Mat redmed, Mat redhigh, ofstream & myfile)
{
	int smalwid = 0, midwid = 0, larwid = 0; int totl = 0, totm = 0, toth = 0; float totwid = 0;
	int LS = 0, MS = 0, HS = 0, LM = 0, MM = 0, HM = 0, LL = 0, ML = 0, HL = 0;// Lowint_Smallwid, Mediumint_MedWid, Highint_LargeWidth
	int denLS = 0, denMS = 0, denHS = 0, denLM = 0, denMM = 0, denHM = 0, denLL = 0, denML = 0, denHL = 0;// density Lowint_Smallwid, Mediumint_MedWid, Highint_LargeWidth
	int TdenLS = 0, TdenMS = 0, TdenHS = 0, TdenLM = 0, TdenMM = 0, TdenHM = 0, TdenLL = 0, TdenML = 0, TdenHL = 0;
	int totleng = 0, avgleng = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{

		Vec4i l = lines[i];
		Point a = (Point(l[0], l[1])); Point b = (Point(l[2], l[3]));
		double w = cv::norm(a - b);
		cv::Rect bRect(a, b);

		Mat lroi = redlow(bRect);// creating a new image from roi of redlow
		int lcount = countNonZero(lroi);
		Mat mroi = redmed(bRect);// creating a new image from roi of redmed
		int mcount = countNonZero(mroi);
		Mat hroi = redhigh(bRect);// creating a new image from roi of redhigh
		int hcount = countNonZero(hroi);
		Mat rect_roi = thresh(bRect);// creating a new image from roi of org green dendrites thresholded
		int wcount = countNonZero(rect_roi);

		if (w > 0)
		{
			totleng = totleng + w;
			int inLcountS; int inMcountS; int inHcountS; int inLcountM; int inMcountM; int inHcountM; int inLcountL; int inMcountL; int inHcountL;
			int widthden = wcount / w; // no of whote pixels in thresholded image/ length of synapse
			if (widthden < 10) // small width dendrite
			{
				smalwid++;
				Mat inlroiS = redlow(bRect);// creating a new image from roi of redlow
				inLcountS = countNonZero(inlroiS);
				Mat inmroiS = redmed(bRect);// creating a new image from roi of redmed
				inMcountS = countNonZero(inmroiS);
				Mat inhroiS = redhigh(bRect);// creating a new image from roi of redhigh
				inHcountS = countNonZero(inhroiS);
				//denLS = inLcountS / (bRect.width* bRect.height);
				//denMS = inMcountS / (bRect.width* bRect.height);
				//denHS = inHcountS / (bRect.width* bRect.height);
				LS = LS + inLcountS;  ///total synase count for low int sysnapses near small width dendrites
				MS = MS + inMcountS;
				HS = HS + inHcountS;


			}
			if (widthden >= 10 && widthden < 20)
			{
				midwid++;
				Mat inlroiM = redlow(bRect);// creating a new image from roi of redlow
				inLcountM = countNonZero(inlroiM);
				Mat inmroiM = redmed(bRect);// creating a new image from roi of redmed
				inMcountM = countNonZero(inmroiM);
				Mat inhroiM = redhigh(bRect);// creating a new image from roi of redhigh
				inHcountM = countNonZero(inhroiM);
				//denLM = inLcountM / (bRect.width* bRect.height);
				//denMM = inMcountM / (bRect.width* bRect.height);
				//denHM = inHcountM / (bRect.width* bRect.height);
				LM = LM + inLcountM;
				MM = MM + inMcountM;
				HM = HM + inHcountM;
			}
			if (widthden >= 20)
			{
				larwid++;
				Mat inlroiL = redlow(bRect);// creating a new image from roi of redlow
				inLcountL = countNonZero(inlroiL);
				Mat inmroiL = redmed(bRect);// creating a new image from roi of redmed
				inMcountL = countNonZero(inmroiL);
				Mat inhroiL = redhigh(bRect);// creating a new image from roi of redhigh
				inHcountL = countNonZero(inhroiL);
				//denLL = inLcountL / (bRect.width* bRect.height);
				//denML = inMcountL / (bRect.width* bRect.height);
				//denHL = inHcountL / (bRect.width* bRect.height);
				LL = LL + inLcountL;
				ML = ML + inMcountL;
				HL = HL + inHcountL;
			}



			totl = lcount + totl;
			totm = mcount + totm;
			toth = hcount + toth;
			totwid = totwid + w;


			TdenLS = TdenLS + denLS; ///total synase density for low int sysnapses near small width dendrites
			TdenMS = TdenMS + denMS;
			TdenHS = TdenHS + denHS;
			TdenLM = TdenLM + denLM;
			TdenMM = TdenMM + denMM;
			TdenHM = TdenHM + denHM;
			TdenLL = TdenLL + denLL;
			TdenML = TdenML + denML;
			TdenHL = TdenHL + denHL;
		}
		else
		{
			cout << "Oh OH OH!!!" << endl;
			denLS = 0; denMS = 0; denHS = 0; denLM = 0; denMM = 0; denHM = 0; denLL = 0; denML = 0; denHL = 0;
		}

	}

	//	cout << midwid << larwid << smalwid << endl;
	int Ftotl, Ftotm, Ftoth, Ftotwid, FavgLS, FavgMS, FavgHS, FavgLM, FavgMM, FavgHM, FavgLL, FavgML, FavgHL;
	//	int avgdenLS, avgdenMS , avgdenHS, avgdenLM , avgdenMM, avgdenHM, avgdenLL, avgdenML, avgdenHL;

	if (lines.size() > 0)
	{
		avgleng = totleng / lines.size();
		Ftotl = totl / lines.size();
		Ftotm = totm / lines.size();
		Ftoth = toth / lines.size();
		Ftotwid = totwid / lines.size();
		if (smalwid > 0)
		{
			FavgLS = LS / smalwid; /// average synase count for low int sysnapses near small width dendrites
			FavgMS = MS / smalwid;
			FavgHS = HS / smalwid;
			/*avgdenLS = TdenLS / smalwid; /// average synase density for low int sysnapses near small width dendrites
			avgdenMS = TdenMS / smalwid;
			avgdenHS = TdenHS / smalwid;*/
		}
		else
		{
			FavgLS = 0; FavgMS = 0; FavgHS = 0;//avgdenLS = 0;avgdenMS = 0;avgdenHS = 0;
		}
		if (midwid > 0)
		{
			FavgLM = LM / midwid;
			FavgMM = MM / midwid;
			FavgHM = HM / midwid;
			/*avgdenLM = TdenLM / midwid;
			avgdenMM = TdenMM / midwid;
			avgdenHM = TdenHM / midwid;*/
		}
		else
		{
			FavgLM = 0; FavgMM = 0; FavgHM = 0; //avgdenLM = 0; avgdenMM = 0; avgdenHM = 0;
		}
		if (larwid > 0)
		{
			FavgLL = LL / larwid;
			FavgML = ML / larwid;
			FavgHL = HL / larwid;
			/*avgdenLL = TdenLL / larwid;
			avgdenML = TdenML / larwid;
			avgdenHL = TdenHL / larwid;*/
		}
		else
		{
			FavgLL = 0; FavgML = 0; FavgHL = 0;// avgdenLL = 0; avgdenML = 0; avgdenHL = 0;
		}


	}
	else
	{
		avgleng = 0; Ftotl = 0; Ftotm = 0; Ftoth = 0; Ftotwid = 0; FavgLS = 0; FavgMS = 0; FavgHS = 0; FavgLM = 0; FavgMM = 0; FavgHM = 0; FavgLL = 0; FavgML = 0; FavgHL = 0; //avgdenLS = 0, avgdenMS = 0, avgdenHS = 0, avgdenLM = 0, avgdenMM = 0, avgdenHM = 0, avgdenLL = 0, avgdenML = 0, avgdenHL = 0;
	}
	myfile << "dendritecalc" << "," << imname << "," << lines.size() << ", " << avgleng << ", " << Ftotl + Ftotm + Ftoth << ", " << Ftotl << "," << Ftotm << "," << Ftoth << "," << Ftotwid << "," << smalwid << "," << midwid << "," << larwid << ", " << FavgLS << "," << FavgMS << "," << FavgHS << "," << FavgLM << "," << FavgMM << "," << FavgHM << "," << FavgLL << "," << FavgML << "," << FavgHL << ",";

}

void maxline(vector<vector<Vec4i>> seglines, string imname, Mat img1, Mat thresh, Mat  redlow, Mat redmed, Mat redhigh, ofstream & myfile)
{

	//cout << "segline size  "<<  seglines.size() << endl;
	//Mat thresh;
	vector<Vec4i>flines; double maxl = 0; int maxpos = 0; vector<double>maxlength; vector<Vec4i> finaldendrites, finalaxons;
	//	cv::inRange(img1, cv::Scalar(0, 25, 0), cv::Scalar(10, 255, 10), thresh);
	for (int i = 0; i < seglines.size(); i++)
	{
		maxl = 0; maxpos = 0;
		if (seglines[i].size() >0)
		{

			for (int j = 0; j < seglines[i].size(); j++)
			{
				Vec4i l = seglines[i][j];
				Point p1 = (l[0], l[1]); Point p2 = (l[2], l[3]);
				double res = cv::norm(p1 - p2);
				if (res > maxl)
				{
					maxpos = j;
					maxl = res;
				}

			}
			flines.push_back(seglines[i][maxpos]);
			maxlength.push_back(maxl);
		}

	}
	RNG rng;

	/*for (size_t i = 0; i < flines.size(); i++)// no of lines within each cluster
	{
	Vec4i l = flines[i];
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	line(img1, Point(l[0], l[1]), Point(l[2], l[3]), color, 3, CV_AA);
	}*/
	finaldendrites.clear();

	myfile << "dendrite" << ",";
	dendritecalc(imname, thresh, flines, redlow, redmed, redhigh, myfile);

}
void segments(vector<double>ang, vector<Vec4i> hough_lines, string imname, Mat img, Mat thresh, Mat  redlow, Mat redmed, Mat redhigh, ofstream & myfile)
{
	int randNum0, randNum1;
	Mat imguntouched = img.clone();
	vector<double> cluster_center; //cluster center value
	int cluster_no[1000]; // maintains cluster number for all lines
	vector<vector<Vec4i>>finalcluster;
	vector<vector<double>> finalangle;
	RNG rng(12345);
	vector<Vec4i> tempor;
	//if there are just 2 lines
	if (hough_lines.size() == 1)
	{
		tempor.push_back(hough_lines[0]);
		finalcluster.push_back(tempor);
	}
	else if ((hough_lines.size() == 2) && (abs(ang[0] - ang[1]) < 10))
	{
		Point a0 = (hough_lines[0][0], hough_lines[0][1]);
		Point b0 = (hough_lines[0][2], hough_lines[0][3]);
		Point a1 = (hough_lines[1][0], hough_lines[1][1]);
		Point b1 = (hough_lines[1][2], hough_lines[1][3]);
		if (norm(a0 - b0) > norm(a1 - b1))//Euclidian distance
		{
			tempor.push_back(hough_lines[0]);//only one line exists
			finalcluster.push_back(tempor);
		}

		else
		{
			tempor.push_back(hough_lines[1]);//only one line exists
			finalcluster.push_back(tempor);
		}
	}
	else//if greater than 2 lines exist
	{
		randNum0 = rand() % (hough_lines.size() - 0) + 0;
		randNum1 = rand() % (hough_lines.size() - 0) + 0;
		cluster_center.push_back(ang[randNum0]);
		cluster_center.push_back(ang[randNum1]);
		cluster_no[randNum0] = 0;
		cluster_no[randNum1] = 1;

		int cencalc[1000];

		for (int co = 0; co < 100; co++)//no of runs of k-means
		{
			for (int i = 0; i < ang.size(); i++)
			{
				double min = 100000; int k = 0; double sum_angleDiff = 0;

				for (int j = 0; j < cluster_center.size(); j++)//check dist with all cluster centers
				{
					double diff = abs(cluster_center[j] - ang[i]);
					sum_angleDiff = sum_angleDiff + diff;
					if (diff < min) //if (difference < smallest difference ) then (min=diff)
					{
						min = diff;
						k = j;
					}
				}
				if (min < (tune*(sum_angleDiff / cluster_center.size()))) // if (min< given threshold) then (put into cluster) 
					cluster_no[i] = k;

				else //else(create new cluster)
				{

					cluster_no[i] = cluster_center.size();
					cluster_center.push_back(ang[i]);
				}

			}
			for (int i1 = 0; i1 < cluster_center.size(); i1++)//update centers of clusters
			{//new cluster center= sum of all angles in a cluster / total no of lines in a cluster
				double summ = 0; int count = 0;
				for (int j1 = 0; j1 < hough_lines.size(); j1++)
				{
					if (cluster_no[j1] == i1)
					{
						summ = summ + ang[j1];
						count = count + 1;
					}
				}
				cluster_center[i1] = summ / count;
			}

		}


		//puttting into vector<vector> based on the indexes(clusters) of the lines
		for (int n = 0; n < cluster_center.size(); n++)
		{
			vector<Vec4i> temp;
			vector<double> tempang;
			for (int m = 0; m < hough_lines.size(); m++)
			{


				if (cluster_no[m] == n)
				{
					tempang.push_back(ang[m]);
					temp.push_back(hough_lines[m]);
				}
			}
			finalcluster.push_back(temp);
			temp.clear();
			finalangle.push_back(tempang);
			tempang.clear();

		}
	}// else ends (run for greater than two lines)

	maxline(finalcluster, imname, imguntouched, thresh, redlow, redmed, redhigh, myfile);

}
//----[6]----------detects dedrite,classifies as dendrite/axon, calc metrics-
void dendritedetect(Mat img, ofstream & myfile, Mat  redlow, Mat redmed, Mat redhigh, string imname, int n)
{

	//==========================================================================================================
	Mat imgclone = img.clone();

	Mat src_gray;
	//HIGH yellow green
	cvtColor(img, src_gray, CV_BGR2GRAY);
	// thresholding bright green and yellow (soma tails/dendrites)
	cv::threshold(src_gray, src_gray, 50, 255, cv::THRESH_TOZERO);
	bitwise_not(src_gray, src_gray);
	cv::GaussianBlur(src_gray, img, cv::Size(3, 3), 0, 0);
	cv::threshold(img, img, 200, 255, cv::THRESH_BINARY);
	bitwise_not(img, img);
	vector<double>angle;
	vector<Vec4i> lines4;
	HoughLinesP(img, lines4, 1, CV_PI / 270, houghthresh, 200, 15);//50 50 10
	for (size_t i = 0; i < lines4.size(); i++)
	{
		Vec4i l = lines4[i];
		double ang = (atan2(l[3] - l[1], l[2] - l[0]) * 180.0 / CV_PI);
		angle.push_back(ang);
	}
	cout << "Number of lines: " << lines4.size() << endl;
	if (lines4.size() > 0)
		segments(angle, lines4, imname, imgclone, img, redlow, redmed, redhigh, myfile);

	//else if(lines4.size() == 1)  finallines=lines4[0];
	else
	{
		//myfile << "No dendrites" << ",";
		myfile << "NOdendritecalc" << "," << "dendritecalc" << "," << imname << "," << 0 << ", " << 0 << ", " << 0 << ", " << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << ", " << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0 << "," << 0;
	}

}



void findsoma(Mat &im, vector<vector<Point>> blue_contours, vector<vector<Point>> intersection_contours, ofstream & myfile)
{

	vector<vector<Point>> astrocytes;
	vector<Vec4i> astrocyteshierarchy;
	vector<vector<Point>> neurons;
	vector<Vec4i> neuronshierarchy;
	vector<vector<Point>> final_soma_contours;
	vector<Vec4i> final_soma_hierarchy;
	vector<vector<Point>> refine_intersect_contours;
	vector<Vec4i> afhierarchy;
	vector<vector<Point>> refine_blue_contours;
	vector<Vec4i> bfhierarchy;
	vector<vector<Point>> scontours;
	vector<Vec4i> shierarchy;
	int soma_indices[100];
	vector<vector<Point>> remcontours;
	vector<Moments> moment_blue(blue_contours.size());
	vector<Point2f> masscenter_blue(blue_contours.size());
	int ib = -1;
	int count = 0;
	vector<Point> som;
	vector<Moments> moment_intersect(intersection_contours.size());
	vector<Point2f> masscenter_intersect(intersection_contours.size());
	int i1 = -1;
	for (int i = 0; i < intersection_contours.size(); i++) // filter areas off intersection contour
	{
		if (fabs(contourArea(intersection_contours[i])) > somathreshold)
			refine_intersect_contours.push_back(intersection_contours[i]);
	}
	for (int i = 0; i < blue_contours.size(); i++) // filter areas of blue contours
	{
		if (fabs(contourArea(blue_contours[i])) > bluecontourthreshold)
			refine_blue_contours.push_back(blue_contours[i]);
	}

	/// Get the moments of blue contours

	for (int i = 0; i < refine_blue_contours.size(); i++)
		moment_blue[i] = moments(refine_blue_contours[i], false);

	///  Get the mass centers of blue contours:
	for (int i = 0; i < refine_blue_contours.size(); i++)
		masscenter_blue[i] = Point2f(moment_blue[i].m10 / moment_blue[i].m00, moment_blue[i].m01 / moment_blue[i].m00);

	/// Get the moments of intersection contours
	for (int i = 0; i < refine_intersect_contours.size(); i++)
		moment_intersect[i] = moments(refine_intersect_contours[i], false);

	///  Get the mass centers of intersection contours:
	for (int i = 0; i < refine_intersect_contours.size(); i++)
		masscenter_intersect[i] = Point2f(moment_intersect[i].m10 / moment_intersect[i].m00, moment_intersect[i].m01 / moment_intersect[i].m00);


	// check if centroid of any of the intersection contours is near the blue contours: If yes then- we have found a soma
	float d = 0; int j1 = 0; int ifin = -1; int j5 = -1; int jfin = 0;
	if (refine_blue_contours.size() == 0)
	{
		myfile << "," << "TOTAL" << "," << 0;
		myfile << "," << "SOMA" << " ," << 0 << " ," << 0 << " ," << 0 << ", " << 0 << " ," << 0;
		myfile << "," << "Astrocyte" << " ," << 0 << " ," << 0 << " ," << 0;
		myfile << "," << "Others" << " ," << 0 << " ," << 0 << " ," << 0;
	}
	else
	{
		for (int i = 0; i < refine_intersect_contours.size(); i++)
		{
			som.clear();
			float dmin = 1000000; int index = 0;
			//cout << "SIZE" << refine_blue_contours.size() << endl;
			for (int j = 0; j < refine_blue_contours.size(); j++)
			{

				d = sqrt(((masscenter_intersect[i].x - masscenter_blue[j].x)*(masscenter_intersect[i].x - masscenter_blue[j].x)) + ((masscenter_intersect[i].y - masscenter_blue[j].y)*(masscenter_intersect[i].y - masscenter_blue[j].y)));

				if (d < dmin)
				{
					dmin = d;
					index = j;

				}
			}

			if (dmin < 200)
			{
				final_soma_contours.push_back(refine_blue_contours[index]);
				soma_indices[i] = index;
			}
		}
		// loop to find remaining contours
		for (int g = 0; g < refine_blue_contours.size(); g++)
		{
			int cc = 0;
			for (int h = 0; h < 100; h++)
			{
				if (g == soma_indices[h])// depicts a soma
					cc++;
			}
			if (cc == 0)
				remcontours.push_back(refine_blue_contours[g]); // not a soma
		}
		int alowcirc = 0, ahighcirc = 0;// circularity of astrocytes
		int nlowcirc = 0, nhighcirc = 0;// circularity of neural cells
		for (int k = 0; k < remcontours.size(); k++)
		{
			cv::RotatedRect min_area_rect = minAreaRect(cv::Mat(remcontours[k]));
			float aspect_ratio = float(min_area_rect.size.width) / min_area_rect.size.height;
			if (aspect_ratio > 1.0) {
				aspect_ratio = 1.0 / aspect_ratio;
			}
			if (aspect_ratio <= 0.8 || aspect_ratio > 1)
			{
				astrocytes.push_back(remcontours[k]);
				if (aspect_ratio <= 0.55)
					alowcirc++;
				else
					ahighcirc++;

			}
			else
			{
				neurons.push_back(remcontours[k]);
				if (aspect_ratio <= 0.55)
					nlowcirc++;
				else
					nhighcirc++;
			}
		}

		int siz;
		vector<Point2f>center(final_soma_contours.size());
		vector<float>radius(final_soma_contours.size());
		vector<float> aspect_ratio(final_soma_contours.size());
		vector<float> diameter;
		float sumaspect = 0;
		float meanaspect = 0;
		float stddevaspect = 0;
		float sumdiameter = 0;
		float meandiameter = 0;
		float stddevdiameter = 0;
		siz = final_soma_contours.size();
		for (int k = 0; k < final_soma_contours.size(); k++)
		{
			minEnclosingCircle((Mat)final_soma_contours[k], center[k], radius[k]);
			circle(im, center[k], radius[k], Scalar(20, 230, 125), 6, 8, 0);
			cv::RotatedRect min_area_rect = minAreaRect(cv::Mat(final_soma_contours[k]));
			aspect_ratio[k] = float(min_area_rect.size.width) / min_area_rect.size.height;
			if (aspect_ratio[k] > 1.0)
				aspect_ratio[k] = 1.0 / aspect_ratio[k];

		}
		for (int k = 0; k < final_soma_contours.size(); k++)
		{
			sumaspect += aspect_ratio[k];
			sumdiameter += (2 * radius[k]);
		}

		meanaspect = sumaspect / siz;
		meandiameter = sumdiameter / siz;

		for (int k = 0; k < final_soma_contours.size(); k++)
		{
			stddevaspect = ((meanaspect - aspect_ratio[k])*(meanaspect - aspect_ratio[k])) / siz;
			stddevdiameter = ((meandiameter - (2 * radius[k]))*(meandiameter - (2 * radius[k]))) / siz;
		}
		myfile << "," << "TOTAL" << "," << refine_blue_contours.size();
		myfile << "," << "SOMA" << " ," << final_soma_contours.size() << " ," << meandiameter << " ," << stddevdiameter << ", " << meanaspect << " ," << stddevaspect;
		myfile << "," << "Astrocyte" << " ," << astrocytes.size() << " ," << alowcirc << " ," << ahighcirc;
		myfile << "," << "Others" << " ," << neurons.size() << " ," << nlowcirc << " ," << nhighcirc;
		for (int j = 0; j < astrocytes.size(); j++)
			drawContours(im, astrocytes, j, Scalar(255, 255, 255), 2, cv::LINE_8, vector<Vec4i>(), 0, Point());
		for (int j = 0; j < neurons.size(); j++)
			drawContours(im, neurons, j, Scalar(255, 125, 255), 2, cv::LINE_8, vector<Vec4i>(), 0, Point());
	}
}
void areabinning(vector<vector<Point>> contours, string str, ofstream &myfile)
{
	int c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = fabs(contourArea((contours[i])));




		if (area >= 5 && area < 100)
			c1++;
		if (area >= 100 && area < 200)
			c2++;
		if (area >= 200 && area < 300)
			c3++;
		if (area >= 300 && area < 400)
			c4++;
		if (area >= 400 && area < 500)
			c5++;
		if (area >= 500)
			c6++;




	}
	myfile << " ," << "SYNAPSE" << "," << str << "," << c1 << "," << c2 << "," << c3 << "," << c4 << "," << c5 << "," << c6 << ",";




}

void synapse(Mat &imm, ofstream &myfile)
{

	vector<vector<Point>> High_synapse_contours;
	vector<Vec4i> thierarchy;
	Mat thr_high_Synapse;
	cv::inRange(imm, cv::Scalar(0, 0, 110), cv::Scalar(30, 30, 255), thr_high_Synapse);//RED-HIGH INTENSITY
	dilate(thr_high_Synapse, thr_high_Synapse, Mat());




	findContours(thr_high_Synapse, High_synapse_contours, thierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); // Find the contours in the image




	for (int i = 0; i < High_synapse_contours.size(); i++)
		drawContours(imm, High_synapse_contours, i, Scalar(128, 0, 255), cv::FILLED, cv::LINE_8, vector<Vec4i>(), 0, Point());//pink-high-purple


	/*~~~~~~~~~~~~*/areabinning(High_synapse_contours, "high intensity", myfile);

	// medium intensity
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Mat Medium_Synapse_thr;
	Mat thrdil;
	dilate(thr_high_Synapse, thrdil, Mat());
	cv::inRange(imm, cv::Scalar(0, 0, 50), cv::Scalar(10, 10, 175), Medium_Synapse_thr);//RED -LOW+HIGH INTENSITY
	Medium_Synapse_thr = Medium_Synapse_thr - thrdil;                // RED (LOW INTENSITY)
	findContours(Medium_Synapse_thr, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); // Find the contours in the image

	for (int i = 0; i < contours.size(); i++)
	{
		drawContours(imm, contours, i, Scalar(255, 255, 0), cv::FILLED, cv::LINE_8, vector<Vec4i>(), 0, Point());//green-medium-dark pink
	}




	/*~~~~~~~~~~~~*/areabinning(contours, "medium intensity", myfile);



	vector<vector<Point>> contoursm;
	vector<Vec4i> hierarchym; Mat Low_Synapse_thr;




	cv::inRange(imm, cv::Scalar(0, 0, 20), cv::Scalar(10, 10, 38), Low_Synapse_thr);
	Low_Synapse_thr = Low_Synapse_thr - Medium_Synapse_thr - thr_high_Synapse;
	//dilate(thrm, thrm, Mat());
	findContours(Low_Synapse_thr, contoursm, hierarchym, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); // Find the contours in the image




	for (int i = 0; i < contoursm.size(); i++)
	{
		drawContours(imm, contoursm, i, Scalar(255, 255, 255), cv::FILLED, cv::LINE_8, vector<Vec4i>(), 0, Point());//orange color-low
	}
	imwrite("Synapse_Show.png", imm);
	/*~~~~~~~~~~~~*/areabinning(contoursm, "low intensity", myfile);
}

vector<vector<Point>> watershedcontours(Mat src)
{
	int noofruns = 30;
	double min, max;

	Point maxLoc;

	vector<vector<Point>> contours_check, large_contours;

	Mat bw, bin;

	vector<Mat> storewatershed;
	//thresholding nuclei (blue) in the image

	Mat bgr[3];   //destination array

	split(src, bgr);//split source

	//Note: OpenCV uses BGR color order

	//threshold(bgr[0], bw, 40, 255, THRESH_BINARY | THRESH_OTSU);// blue channel threshold
	cv::inRange(bgr[0], cv::Scalar(50, 2, 5), cv::Scalar(250, 20, 25), bw);//blue threshold	
	dilate(bw, bw, Mat());
	//dilate(bw, bw, Mat());
	//imshow("Binary Image", bw);

	Mat kernel_op = getStructuringElement(MORPH_RECT, Size(3, 3));

	Mat morph1;

	morphologyEx(bw, morph1, CV_MOP_OPEN, kernel_op);

	//imshow("openmorphology", morph1);

	Mat morph;

	Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

	morphologyEx(morph1, morph, CV_MOP_CLOSE, kernel2);

	//erode(morph, morph, Mat());

	//imshow("after morphology", morph);

	bw = morph;

	// Perform the distance transform algorithm

	Mat dist;

	distanceTransform(bw, dist, CV_DIST_L2, 3);

	normalize(dist, dist, 0, 1., NORM_MINMAX);

	//imshow("Distance Transform Image", dist);

	threshold(dist, dist, .2, 1., CV_THRESH_BINARY);

	// Dilate a bit the dist image

	Mat kernel1 = Mat::ones(3, 3, CV_8UC1);

	dilate(dist, dist, kernel1);

	//imshow("Peaks", dist);

	//cout << countNonZero(dist) << endl;

	if (countNonZero(dist) > 70000)

		noofruns = 20;

	// Create the CV_8U version of the distance image

	// It is needed for findContours()

	Mat dist_8u, distback_8u;

	dist.convertTo(dist_8u, CV_8U);

	// Find total markers

	vector<vector<Point> > contours, backcontours;

	findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	Mat dist_back;

	threshold(dist, dist_back, 0, 0.5, CV_THRESH_BINARY_INV);

	Mat kern_erod = getStructuringElement(MORPH_RECT, Size(5, 5));

	for (int y = 0; y < noofruns; y++)
	{
		erode(dist_back, dist_back, kern_erod);
	}


	//imshow("PeaksBackground", dist_back);

	dist_back.convertTo(distback_8u, CV_8U);

	findContours(distback_8u, backcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	Mat markers = Mat::zeros(dist.size(), CV_32SC1);

	// Draw the foreground markers

	for (size_t i = 0; i < contours.size(); i++)

	{
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);
	}

	for (size_t i = 0; i < backcontours.size(); i++)
		drawContours(markers, backcontours, (i), Scalar(255, 255, 255), -1);





	int ncomp = contours.size();

	//imshow("Markers", markers * 10000);

	// Perform the watershed algorithm

	watershed(src, markers);

	vector<Vec3b> colors;

	for (size_t i = 0; i < contours.size(); i++)

	{
		int b = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int r = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}


	vector<Mat> images2; vector<vector<Point> > final_contours, all_finalcontours;

	for (int obj = 1; obj < contours.size() + 1; obj++)

	{

		Mat dst2;

		src.copyTo(dst2, (markers == obj));

		cv::inRange(dst2, cv::Scalar(40, 0, 0), cv::Scalar(255, 55, 45), dst2);//BLUE THRESH

		morphologyEx(dst2, dst2, CV_MOP_CLOSE, kernel2);

		morphologyEx(dst2, dst2, CV_MOP_CLOSE, kernel2);

		findContours(dst2, final_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		for (int u = 0; u < final_contours.size(); u++)

			all_finalcontours.push_back(final_contours[u]);

		images2.push_back(dst2.clone());

	}




	for (size_t p = 0; p < all_finalcontours.size(); p++)

	{

		Scalar rnd_colors = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(src, all_finalcontours, p, rnd_colors, cv::FILLED, cv::LINE_8, vector<Vec4i>(), 0, Point());

	}

	return all_finalcontours;

}
int main(int argc, char** argv)
{

	std::string file_contents;
	myfile.open("STEP1_waste.csv");
	std::ifstream file("oldstep.txt");
	std::string fstr; string name;
	vector<vector<Point>> blue_contours;
	vector<Vec4i> bhierarchy;
	vector<vector<Point>> intersect_contours;
	vector<vector<Point>> mod_intersect_contours;
	vector<Vec4i> ahierarchy;
	Mat bthr, src_gray, gth; Mat added; Mat intersection;
	using namespace cv;
	vector<vector<Point>> tcontours;
	vector<Vec4i> thierarchy;

	// Read image

	while (std::getline(file, fstr))
	{
		// Read image
		for (int n = 1; n <= 40; n++)
		{
			if (n < 10)
				name = format("C:\\CCHMC\\NGN2\\Old STEP1\\%s\\z%d_3layers_original.tif", fstr.c_str(), n);
			else
				name = format("C:\\CCHMC\\NGN2\\Old STEP1\\%s\\z%d_3layers_original.tif", fstr.c_str(), n);

			cout << format("Processsing Folder %s", fstr.c_str()) << endl;
			Mat imm = imread(name);
			Mat imcopy = imm.clone();
			Mat imtofn = imm.clone();

			if (imm.empty()) //if image is empty- read next line of txt file- move on to the next folder
			{
				cout << "NOT FOUND:" << endl; goto POP;
			}
			//string result = format("C:\\Users\\VIneeta\\Documents\\Visual Studio 2013\\Projects\\OpenCV-Test\\OpenCV-Test\\Old STEP1i\\%s\\z%d_3layers_processed.tif", fstr.c_str(), n);
			string result = format("C:\\CCHMC\\NGN2\\Old STEP1\\%s\\%s_z%dc1+2+3_processed.jpg", fstr.c_str(), fstr.c_str(), n);
			string imagnam = format("%s_z%dc1+2+3", fstr.c_str(), n);
			myfile << imagnam;
			cout << format("Processsing Image %s", imagnam.c_str()) << endl;

			Mat redlow, redmed, redhigh;

			Mat thr_high_Synapse;
			cv::inRange(imm, cv::Scalar(0, 0, 110), cv::Scalar(30, 30, 255), redhigh);//RED-HIGH INTENSITY
			dilate(redhigh, redhigh, Mat());
			//imshow("thr_high_Synapse", thr_high_Synapse);

			Mat Medium_Synapse_thr;
			cv::inRange(imm, cv::Scalar(0, 0, 50), cv::Scalar(10, 10, 175), redmed);//RED -MED INTENSITY
			dilate(redmed, redmed, Mat());

			Mat Low_Synapse_thr;
			cv::inRange(imm, cv::Scalar(0, 0, 20), cv::Scalar(10, 10, 38), redlow);
			redlow = redlow - redmed - redhigh;//RED -LOW INTENSITY
			dilate(redlow, redlow, Mat());

			cvtColor(imm, src_gray, CV_BGR2GRAY);

			/*~~~~~~~*/ synapse(imm, myfile);

			/*~~~~~~~*/dendritedetect(imcopy, myfile, redlow, redmed, redhigh, fstr, n);


			// thresholding bright green and yellow (soma tails/dendrites)
			cv::threshold(src_gray, src_gray, 50, 255, cv::THRESH_TOZERO);
			bitwise_not(src_gray, src_gray);
			cv::GaussianBlur(src_gray, gth, cv::Size(3, 3), 0, 0);
			cv::threshold(gth, gth, 200, 255, cv::THRESH_BINARY);
			bitwise_not(gth, gth);
			dilate(gth, gth, Mat());

			//thresholding nuclei (blue) in the image
			Mat bgr[3];   //destination array
			split(imm, bgr);//split source  
			//Note: OpenCV uses BGR color order
			cv::inRange(bgr[0], cv::Scalar(50, 2, 5), cv::Scalar(250, 20, 25), bthr);//blue threshold	
			dilate(bthr, bthr, Mat());
			imwrite("bthr.png", bthr);
			cout << "written" << endl;
			bitwise_and(gth, bthr, intersection);//intersection of nuclei with dendrite
			//findContours(bthr, blue_contours, bhierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); // Find the blue contours in the image
			findContours(intersection, intersect_contours, ahierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); // Find the contours in the intersection image

			/*~~~~~~~*/blue_contours = watershedcontours(imtofn);

			/*~~~~~~~*/ findsoma(imm, blue_contours, intersect_contours, myfile);

			//-------------------------------------------------------------------------------------------------------------------------------------------   

			imwrite(result, imm);
			myfile << endl;


		}
	POP:;
	}
	myfile.close();
	waitKey(0);
}


