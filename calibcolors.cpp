#include "calibcolors.h"
void calibrateColors(int nr){
    CalibColors*c = new CalibColors();
    c->calibrate(nr);
    delete c;
}
CalibColors::CalibColors()
{
    fps=camProps.fps;
    exposure=100*camProps.exposure;
    gain=100*camProps.gain;
    proc = new Processor(fps);
}
void CalibColors::onChangeHSV(int newValue, void *object){

}
void CalibColors::onChange(int newValue, void * object){
    CalibColors* obj = (CalibColors*) object;
    switch(obj->fps){
        case 0:
            obj->fps=30;
            break;
        case 1:
            obj->fps=45;
            break;
        case 2:
            obj->fps=60;
            break;
        case 3:
            obj->fps=75;
            break;
        case 4:
            obj->fps=100;
            break;
    }
    obj->camProps.fps=obj->fps;
    obj->camProps.exposure=(float)obj->exposure/100;
    obj->camProps.gain=(float)obj->gain/100;
    obj->cap.set(CV_CAP_PROP_FPS,obj->camProps.fps);
    obj->cap.set(CV_CAP_PROP_GAIN,obj->camProps.gain);
    obj->cap.set(CV_CAP_PROP_EXPOSURE,obj->camProps.exposure);
}
void CalibColors::calibrate(int nr){
    Mat display;
    Mat frame;
    Mat color;
    Mat torgb;
    cap.open(nr +CAP_V4L2);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    cap.set(CV_CAP_PROP_FPS,camProps.fps);
    cap.set(CV_CAP_PROP_GAIN,camProps.gain);
    cap.set(CV_CAP_PROP_EXPOSURE,camProps.exposure);

    namedWindow("Kalibracja kolorow");
    createTrackbar("FPS","Kalibracja kolorow", &fps, 4,CalibColors::onChange,this);
    createTrackbar( "Gain","Kalibracja kolorow", &gain, 100,CalibColors::onChange,this);
    createTrackbar( "Exposure","Kalibracja kolorow",  &exposure, 100,CalibColors::onChange,this);
    createTrackbar("H - Low","Kalibracja kolorow", &procProps.lowH, 255,CalibColors::onChangeHSV,this);
    createTrackbar("S - Low","Kalibracja kolorow", &procProps.lowS, 255,CalibColors::onChangeHSV,this);
    createTrackbar("V - Low","Kalibracja kolorow",  &procProps.lowV, 255,CalibColors::onChangeHSV,this);
    createTrackbar("H - High","Kalibracja kolorow", &procProps.highH, 255,CalibColors::onChangeHSV,this);
    createTrackbar("S - High","Kalibracja kolorow", &procProps.highS, 255,CalibColors::onChangeHSV,this);
    createTrackbar("V - High","Kalibracja kolorow",  &procProps.highV, 255,CalibColors::onChangeHSV,this);
    /*cv::VideoWriter output("wideo.avi",
                   CV_FOURCC('P','I','M','1'),
                   50,
                   cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
                   cap.get(CV_CAP_PROP_FRAME_HEIGHT)));*/

    while(1){

        cap>>frame;
        proc->test(frame,procProps);
        proc->mask.download(color);
        cvtColor(color,torgb,CV_GRAY2BGR);
        hconcat(frame,torgb,display);
        imshow("Kalibracja kolorow",display);

        char c=waitKey(1);
        if( c== 27) break;
        if( c== 32){
            camProps.save();
            std::cout<<"Zapisano parametry kamery"<<std::endl;
            procProps.save();
            std::cout<<"Zapisano parametry procesora"<<std::endl;
            break;
        }

    }
    //output.release();
    delete proc;
    frame.release();
    display.release();
    frame.release();
    color.release();
    torgb.release();
    cap.release();
    destroyAllWindows();
}

