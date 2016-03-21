#include <iostream>
#include <fstream>
#include <opencv/highgui.h>

#define ROI_SZ 100
#define ROI_CANVAS ROI_SZ*0.35

struct Cell
{
    int idx;
    char val;
    char type;
};

void storeCell(std::string word, std::map<int,Cell>& board_colors)
{
    Cell cell;
    cell.idx = (int)word[word.length()-4]-(int)48;
    if(isdigit(word[word.length()-5]))
    {
        cell.idx = cell.idx + ((int)word[word.length()-5] -(int)48)*10;
        cell.type = word[word.length()-7];
    }
    else cell.type = word[word.length()-6];

    cell.val = word[word.length()-2];
    board_colors.insert(std::make_pair(cell.idx,cell));
}

// gringo file.lp | clasp 1 >> asp_solver.ou
void load(std::map<int,Cell>& board_colors)
{
    std::ifstream ans_set;
    ans_set.open( "asp_solver.out", std::ifstream::in);
    if(ans_set.is_open())
    {
        bool next = false;
        while(!ans_set.eof())
        {
            std::string tmp_ln;
            getline(ans_set,tmp_ln);
            std::stringstream iss(tmp_ln);

            std::string word_tmp;
            while(iss >> word_tmp)
            {
                if("Answer:" == word_tmp)
                {
                    next = true;
                    break;
                }

                if("SATISFIABLE" == word_tmp || "UNSATISFIABLE" == word_tmp)
                {
                    next = false;
                    break;
                }

                if(next) storeCell(word_tmp, board_colors);
                else continue;
            }
        }
    }
    ans_set.close();

}

void number(cv::Mat img, int _num, cv::Point2f _p, cv::Scalar color, cv::Point2f displacement = cv::Point2i())
{
    std::stringstream ss;
    ss << _num;
    std::string num = ss.str();

    float fontScale = 0.5f;
    int thickness = 1;

    cv::putText(img,num,_p+displacement,cv::FONT_HERSHEY_SCRIPT_SIMPLEX,fontScale,color,thickness);
}

int main()
{
    std::map<int,Cell> board_colors;
    load(board_colors);
    cv::Mat board;

    if(board_colors.size() != 0)
    {
        int board_side = sqrt(board_colors.size());
        board = cv::Mat::zeros(board_side*ROI_SZ,board_side*ROI_SZ,CV_8UC3);

        std::map<int,Cell>::const_iterator drawer = board_colors.begin();
        for(; drawer != board_colors.end(); ++drawer)
        {
            int r = (drawer->first-1)/board_side;
            int c = (drawer->first-1)%board_side;

            if(drawer->second.type == 't')
            {
                for(unsigned int r_roi = r*ROI_SZ+ROI_CANVAS; r_roi < r*ROI_SZ + ROI_SZ-ROI_CANVAS; ++r_roi)
                {
                    for(unsigned int c_roi = c*ROI_SZ+ROI_CANVAS; c_roi < c*ROI_SZ + ROI_SZ-ROI_CANVAS; ++c_roi)
                    {
                        if((drawer->second.val) == 'r') board.at<cv::Vec3b>(r_roi,c_roi) = cv::Vec3b(51,51,255);
                        else if((drawer->second.val) == 'g') board.at<cv::Vec3b>(r_roi,c_roi) = cv::Vec3b(51,255,51);
                        else if((drawer->second.val) == 'b') board.at<cv::Vec3b>(r_roi,c_roi) = cv::Vec3b(255,153,51);
                        else if((drawer->second.val) == 'y') board.at<cv::Vec3b>(r_roi,c_roi) = cv::Vec3b(51,255,255);
                        else if((drawer->second.val) == 'p') board.at<cv::Vec3b>(r_roi,c_roi) = cv::Vec3b(255,51,153);
                    }
                }
            }
            else if(drawer->second.type == 'e')
            {
                cv::Scalar color = cv::Scalar::all(0);
                if((drawer->second.val) == 'r') color = cv::Scalar(51,51,255);
                else if((drawer->second.val) == 'g') color = cv::Scalar(51,255,51);
                else if((drawer->second.val) == 'b') color = cv::Scalar(255,153,51);
                else if((drawer->second.val) == 'y') color = cv::Scalar(51,255,255);
                else if((drawer->second.val) == 'p') color = cv::Scalar(255,51,153);

                cv::circle( board, cv::Point2i(c*ROI_SZ+ROI_SZ/2, r*ROI_SZ+ROI_SZ/2), ROI_SZ/3, color, CV_FILLED);
            }

            number(board, drawer->first, cv::Point2i(c*ROI_SZ+ROI_SZ/2, r*ROI_SZ+ROI_SZ/2), cv::Scalar::all(255), cv::Point2i(-ROI_SZ/2 +10, -ROI_SZ/2 +20));
        }

        for(unsigned int ln = ROI_SZ; ln < board_side*ROI_SZ; ln+=ROI_SZ)
        {
            cv::line( board, cv::Point2i(0,ln), cv::Point2i(board_side*ROI_SZ,ln), cv::Scalar::all(255), 2, 1);
            cv::line( board, cv::Point2i(ln,0), cv::Point2i(ln,board_side*ROI_SZ), cv::Scalar::all(255), 2, 1);
        }
    }
    else
    {
        int rows = 280;
        int cols = 340;
        board = cv::Mat::zeros(rows,cols,CV_8UC3);

        cv::putText(board,"UNSATISFIABLE",cv::Point2i(rows/2-30,cols/2-50),CV_FONT_NORMAL,0.5f,cv::Scalar::all(255),1);
    }



    cv::imshow("Board", board);
    cv::waitKey(0);

    return 0;
}
