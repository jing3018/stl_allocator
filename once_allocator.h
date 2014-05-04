/*
 * =====================================================================================
 * 
 *       Filename:  once_allocter.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2014-02-07 14:09:18
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zijia (search engine group), zijia@taobao.com
 *        Company:  www.taobao.com
 * 
 * =====================================================================================
 */

#ifndef  ONCE_ALLOCTER_INC
#define  ONCE_ALLOCTER_INC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allocator.h"

/*Only allocate, no free
 * */
class OnceAllocator : public Allocator {
    public:
        //default size is 100 Mega Bytes
        static const size_t DEFAULT_SIZE    = 100*1024*1024;
        //can only expand 10 times, to 100 Mega * 1K = 100G bytes
        static const int    MAX_EXPAND_TIME = 10;

        OnceAllocator(){
            _type = AllocOnce;
            _cur_block = 0;
            for(int i=0; i<MAX_EXPAND_TIME; i++){
                _data[i] = NULL;
            } 
            _data[0]   = (char*)malloc(DEFAULT_SIZE);
            _cur_head  = _data[0];
            _total_size = DEFAULT_SIZE;
            _empty_size = DEFAULT_SIZE;
            _used_size  = 0;
        }

        ~OnceAllocator(){
            release();
        }

        //Restrictions:
        //1,only allocates continuous memory, memory cannot span two blocks
        //2,if "size" > current-block-size, it fails, so the max "size" is DEFAULT_SIZE
        //SideEffects:
        //1,there will be memory hole in block, for eg:
        //  step1: alloc 1 byte 
        //  step2: alloc 100M bytes
        //  there will be (100M-1) bytes hole in block 0
        char *alloc(unsigned int size){
            size_t need_size = size + sizeof(unsigned int);
            if(need_size>DEFAULT_SIZE)
                return NULL;

            if(need_size>_empty_size){
                if(expand() == false){
                    return NULL;
                }
            }

            char *r = _cur_head;
            //save the length information
            *(unsigned int*)r = size;
            _cur_head   += need_size;
            _used_size  += need_size;
            _empty_size -= need_size;
            return r+sizeof(unsigned int);
        }

        char *realloc(char *src, unsigned int size){
            if(!src) 
                return NULL;
            char *dest = alloc(size);
            if(!dest)
                return NULL;
            unsigned int src_size = *(unsigned int*)(src-sizeof(unsigned int));
            memcpy(dest, src, src_size);
            _used_size -= src_size + sizeof(unsigned int);
            return dest;
        }

        //no free, so not implement it
        virtual void free(char* src){
        }

        //release all memory it holds
        virtual void release(){
            for(int i=0; i<MAX_EXPAND_TIME; i++){
                if(_data[i]){
                    free(_data[i]);
                    _data[i] = NULL;
                }
            } 
        }

        //release all memory; then prepare for next allocation
        void reset(){
            release();
            _cur_block = 0;
            _data[0]   = (char*)malloc(DEFAULT_SIZE);
            _cur_head  = _data[0];
            _total_size = DEFAULT_SIZE;
            _empty_size = DEFAULT_SIZE;
            _used_size  = 0;
        }

    void debug(){
        printf("total_size:%lld\n \
                empty_size:%lld\n \
                used_size:%lld\n \
                hole_size:%lld\n \
                block_num:%d\n",
                _total_size, 
                _empty_size, 
                _used_size, 
                (_total_size-_used_size-_empty_size), 
                (_cur_block+1));
    }

    private:
        //expand to *2
        bool expand(){
            char *buf = (char*)malloc(_total_size);;
            if(!buf){
                fprintf(stderr, "expand _data[%d] to size %lu failed.\n", (_cur_block+1), _total_size);
                return false;
            }
            _data[++_cur_block] = buf;
            _empty_size = _total_size;
            _total_size *= 2;
            _cur_head = _data[_cur_block];
            return true;
        }

    private:
        char*   _data[MAX_EXPAND_TIME]; //real memory
        int     _cur_block;     //current block id
        char*   _cur_head;      //start pos for next allocation
        size_t  _total_size;    //total size block0 + block1 + ...
        size_t  _empty_size;    //current empty size 
        size_t  _used_size;     //current used size
};

#endif   /* ----- #ifndef ONCE_ALLOCTER_INC  ----- */
