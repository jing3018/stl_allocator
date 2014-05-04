/*
 * =====================================================================================
 * 
 *       Filename:  allocator.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2014-02-07 16:20:30
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zijia (search engine group), zijia@taobao.com
 *        Company:  www.taobao.com
 * 
 * =====================================================================================
 */

#ifndef  USER_ALLOCATOR_INC
#define  USER_ALLOCATOR_INC

enum AllocType {
    AllocNone,
    AllocC,
    AllocCpp,
    AllocOnce
};

/*Base Class for mutiple types of Allocator
 *Allocator allocates/free memory instead of glibc malloc/free or Cpp new/delete
 * */
class Allocator {
    public:
        //default none
        Allocator(){
            _type = AllocNone;
        }

        virtual ~Allocator(){
        }
        
        //alloc as malloc/new
        virtual char* alloc(unsigned int size){
            return NULL;
        }

        //realloc as glibc realloc
        virtual char* realloc(char *src, unsigned int size){
            return NULL;
        }

        //free as free/delete
        virtual void free(char *src){
        }

        //release all memory it holds
        virtual void release(){
        }

        AllocType type(){
            return _type;
        }

        void debug(){
        }

    protected:
        AllocType _type;
};

/*Do nothing for user.  does malloc/free or new/delete himself.
 * */
class NoneAllocator : public Allocator {
};

/*Encapsulation for c++ new/delete
 * */
class CppAllocator : public Allocator {
    public:
        CppAllocator(){
            _type = AllocCpp;
        }

        //s call construction methods themselves
        //Here, just allocates memory
        virtual char* alloc(unsigned int size){
            return new char[size];
        }

        //Attention! not implement realloc for c++
        virtual char* realloc(char *src, unsigned int size){
            return NULL;
        }

        //s call destruction methods themselves
        //Here, just free memory
        virtual void free(char *src){
            delete[] src;
        }
};

/*Encapsulation for glibc malloc/free
 * */
class CAllocator : public Allocator {
    public:
        CAllocator(){
            _type = AllocC;
        }

        virtual char* alloc(unsigned int size){
            return (char*)malloc(size);
        }

        virtual char* realloc(char *old, unsigned int size){
            return (char*)realloc(old, size);
        }

        virtual void free(char *src){
            free(src);
        }
};
#endif   /* ----- #ifndef ALLOCATOR_INC  ----- */
