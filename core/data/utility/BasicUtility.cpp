
#include<cmath>
#include"core/data/utility/Configuration.h"


#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include "core/data/mat/MatN.h"
#include"core/data/utility/BasicUtility.h"

#if Pop_OS==2
#ifdef _MSC_VER
#include"core/dependency/direntvc.h"
#include <direct.h>
#else
#include <direct.h>
#include <dirent.h>
#endif
#else
#include <dirent.h>
#endif

namespace pop
{
std::string BasicUtility::IntFixedDigit2String(unsigned int value,int digitnumber)
{
    long int  number =std::pow (10.,digitnumber);
    long int tempvalue = value/number;
    value-=tempvalue*number;
    std::string s;
    for(int i =digitnumber-1;i>=0;i--)
    {
        long int number =std::pow (10.,i);
        long int tempvalue = value/number;
        value-=tempvalue*number;
        std::string str;
        Any2String(tempvalue,str);
        s+=str;
    }
    return s;
}
std::string BasicUtility::getline(std::istream& in,std::string del){


    std::string str;
    std::string temp;
    int index=0;
    char c;
    while(index<(int)del.size()){
        c = in.get();
        if (in.good()){
            if(c==del.operator [](index))
            {
                temp+=c;
                index++;
            }
            else
            {
                if(index!=0){
                    str+=temp;
                    temp.clear();
                    index=0;
                }
                if(c==del.operator [](index))
                {
                    temp+=c;
                    index++;
                }
                else
                    str+=c;
            }
        }
    }
    return str;
}
std::string BasicUtility::getExtension(std::string file){
    int size =0, index = 0;

    do{
        if(file[size] == '.') {
            index = size;
        }
        size ++;
    }while(size<(int)file.size());

    if(size && index)
        return file.assign(file.begin()+index,file.end());
    else
        return "";
    //throw(pexception("BasicUtility::getExtension, no extension in your file :" +file));
}
std::string BasicUtility::getBasefilename(std::string file){
    int slash, dot;
    slash = -1;
    dot = file.size()-1;
    int size =0;
    while(size ++, file[size]) {
        if(file[size] == '.') {
            dot = size;
        }
        if(file[size] == '/'||file[size] == '\\') {
            slash = size;
        }
    }
    return file.assign(file.begin()+slash+1,file.begin()+dot);

}
std::string BasicUtility::getPath(std::string file){
    int slash;
    slash = -1;
    int size =0;
    while(size ++, file[size]) {
        if(file[size] == '/'||file[size] == '\\') {
            slash = size;
        }
    }
    return file.assign(file.begin(),file.begin()+slash);

}

std::vector<std::string> BasicUtility::getFilesInDirectory(std::string dir)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        return std::vector<std::string>();
        //        pexception("BasicUtility::getFilesInDirectory, Cannot open this directory"+dir);
    }
    std::vector<std::string> files;
    while ((dirp = readdir(dp)) != NULL) {
        std::string str  = std::string(dirp->d_name);
        if(str!="."&&str!=".."&&*str.rbegin()!='~')
            files.push_back(str);
    }
    closedir(dp);
    std::sort(files.begin(),files.end());
    return files;

}
bool BasicUtility::isFile( std::string filepath){

    int status;
    struct stat st_buf;
    status = stat (filepath.c_str(), &st_buf);
    if (status != 0) {
        return false;
    }

    if (S_ISREG (st_buf.st_mode)) {
        return true;
    }
    else
        return false;
}

bool BasicUtility::isDirectory(std::string dirpath){
    //#if Pop_OS==1
    ////    if ( access( dirpath.c_str(), 0 ) == 0 )
    //#endif
    {
        struct stat status;
        stat( dirpath.c_str(), &status );

        if ( status.st_mode & S_IFDIR )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    //#if Pop_OS==1
    //    else
    //#endif
    //    {
    //        return false;
    //    }
}

bool BasicUtility::makeDirectory(std::string dirpath){

    if(isDirectory(dirpath)==false){
#if Pop_OS==1
        mkdir(dirpath.c_str(),777);
#endif
#if Pop_OS==2
        _mkdir(dirpath.c_str());
#endif
        return true;
    }
    else
        return false;
}

struct operatorReplaceSlashByAntiSlash
{
    char operator()(char c){
        if(c=='/')
            return '\\';
        else
            return c;
    }
};
std::string BasicUtility::replaceSlashByAntiSlash(std::string filepath){
    std::transform(filepath.begin(),filepath.end(),filepath.begin(),operatorReplaceSlashByAntiSlash());
    return filepath;
}
int BasicUtility::editDistance(std::string s1,std::string s2){
    pop::Mat2F64 m(s1.size(),s2.size());
    for(unsigned int i =0;i<m.sizeI();i++){
        m(i,0)=i;
    }
    for(unsigned int j =0;j<m.sizeJ();j++){
        m(0,j)=j;
    }
    int k =2;
    bool test=true;
    while(test==true){
        test=false;
        for(int i=1;i<=k-1;i++){
            int j=k-i;
            if(m.isValid(i,j)){
                test =true;
                int v=1;
                if(s1[i-1]==s2[j-1])
                    v=0;

                int v1;
                if(m(i,j-1)+1<m(i-1,j)+1)
                    v1 = m(i,j-1)+1;
                else
                    v1 = m(i-1,j)+1;
                if(v1<m(i-1,j-1)+v)
                    m(i,j)= v1;
                else
                    m(i,j)= m(i-1,j-1)+v;
            }
        }
        k++;
    }
    return m(m.sizeI()-1,m.sizeJ()-1);
}
void BasicUtility::cryptOrDecryptFileXORKey(std::string inputfile,std::string outputfile, pop::UI32 key ){


    std::ifstream  infile(inputfile.c_str(), std::ios::binary);
    infile.seekg (0,infile.end);
    long size = infile.tellg();
    infile.seekg (0);
    char * c = new char[size];
    infile.seekg( 0, std::ios::beg);
    infile.read(c, size);
    pop::UI32 * ptrUI32 =reinterpret_cast<pop::UI32*>(c);
    for(unsigned int i =0;i<std::floor(size/4.);i++){
        *ptrUI32 = (*ptrUI32)^(key);
        ptrUI32++;
    }
    std::ofstream outfile (outputfile.c_str(),std::ofstream::binary);

    outfile.write (c,size);
    delete[] c;

    outfile.close();
    infile.close();
}
}
//namespace std{
//std::istream& operator >> (std::istream& in, unsigned char & c)
//{
//    int v;
//    in>>v;
//    c = (unsigned char)v;
//    return in;
//}

//std::ostream& operator << (std::ostream& out, unsigned char  c)
//{
//    out<<(int)c;
//    return out;
//}


//}
