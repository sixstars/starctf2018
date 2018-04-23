/*
 * heap.c
 * Copyright (C) 2018 hzshang <hzshang15@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

/* 64 bit heap malloc & free */
#include "heap.h"

chunk_ptr* bins_ptr = NULL;

// install chunk to bins
static void install_chunk(chunk_ptr ptr) {
    size_t size = ptr->size;
    if(size == 0) {
        //fprintf(stderr, "%p:invalid chunk!\n", ptr);
        exit(-1);
    }
    chunk_ptr* head;
    if(size > MAX_SMALL_BIN_SIZE) {
        //install to first array
        head = bins_ptr;
    } else {
        head = bins_ptr + index_of_bins(size);
    }
    ptr->next = *head;
    *head = ptr;
}
// uninstall chunk from bins
static chunk_ptr uninstall_chunk(chunk_ptr ptr) {
    size_t size = ptr->size;
    if(size == 0) {
        //fprintf(stderr, "%p:invalid chunk!\n", ptr);
        exit(-1);
    }

    int index= size > MAX_SMALL_BIN_SIZE ? 0:index_of_bins(size);
    
    chunk_ptr temp=bins_ptr[index];
    if(temp == ptr) {
        bins_ptr[index] = ptr->next;
    } else {
        while(temp) {
            if(temp->next == ptr)
                break;
            temp = temp->next;
        }
        if(!temp) {
            //fprintf(stderr, "%p:no such chunk in bins!\n", ptr);
            exit(-1);
        } else {
            temp->next = ptr->next;
        }
    }
    return ptr;
}

// init heap
int heap_init() {
    // init bins
    bins_ptr = (chunk_ptr *)mmap(0, BINS_SIZE * sizeof(chunk_ptr), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(bins_ptr == NULL)
        exit(-1);
    memset(bins_ptr, 0, BINS_SIZE * sizeof(chunk_ptr));

    // mmap a heap for the first big chunk
    chunk_ptr first_heap = (chunk_ptr)mmap(0, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(first_heap == NULL)
        exit(-1);
    memset(first_heap, 0, HEAP_SIZE);
    update_chunk(first_heap, HEAP_SIZE, 0, NOUSE | MAP);
    install_chunk(first_heap);
}


void* mymalloc(size_t size) {
    if(!size)
        return 0;
    size_t chunk_size = mem_chunk_size(size);
    chunk_ptr ret;
    int index = 0;
    if (chunk_size <= MAX_SMALL_BIN_SIZE ) {
        index = index_of_bins(chunk_size);
        if(bins_ptr[index] == 0) {
            index = 0;
        }
    }
    ret = malloc_from_bins(index, chunk_size);
    if(ret)
        return chunk_content(ret);
    else
        return NULL;
}

int myfree(void* dev) {
    chunk_ptr ptr = get_chunk_by_content(dev);
    // never free a mmap chunk
    if(chunk_is_map(ptr)) {
        //fprintf(stderr, "%p can't free mmaped chunk\n", dev);
        exit(-1);
    }
    update_chunk_flag(ptr,NOUSE|NOMAP);

    chunk_ptr up = chunk_up(ptr);
    chunk_ptr down = chunk_down(ptr);
    if(up != ptr && !chunk_is_use(up)) {
        uninstall_chunk(up);
        ptr = merge_chunk(up, ptr);
    }
    if(!chunk_is_use(down)) {
        uninstall_chunk(down);
        ptr = merge_chunk(ptr, down);
    }

    install_chunk(ptr);
    return 0;
}

//merge two free chunk
static chunk_ptr merge_chunk(chunk_ptr up, chunk_ptr down) {
    int is_down_map=chunk_is_map(down);
    size_t size = up->size +down->size;
    size_t last_size = chunk_last_size(up);
    int flag = chunk_flag(up) | chunk_flag(down);
    // init new flag
    chunk_ptr new_chunk = up;
    update_chunk(new_chunk,size,last_size,flag);
    
    // change next chunk's last size
    if(!is_down_map) {
        chunk_ptr temp = chunk_down(new_chunk);
        int temp_flag = chunk_flag(temp);
        update_chunk(temp,temp->size,size,temp_flag);
    }
    return new_chunk;
}

static void* malloc_from_bins(int index, size_t size) {
    if(!bins_ptr[index]) {
        // no more chunk
        return NULL;
    }
    chunk_ptr new_chunk;
    //index > 0 , cut from fast bins
    if(index > 0) {
        new_chunk = uninstall_chunk(bins_ptr[index]);
        update_chunk_flag(new_chunk, USE | NOMAP);
        return new_chunk;
    }
    //index == 0 , cut from big chunk

    //find the chunk
    chunk_ptr temp = bins_ptr[0];
    while(temp) {
        if(temp->size >= size)
            break;
        temp = temp->next;
    }

    //malloc size is too big
    if(temp == 0)
        return NULL;

    // uninstall chunk first
    chunk_ptr uninstalled_chunk = uninstall_chunk(temp);
    int ismap = chunk_is_map(uninstalled_chunk);
    size_t total_size = uninstalled_chunk->size;
    size_t last_size = chunk_last_size(uninstalled_chunk);

    //init new chunk
    new_chunk = uninstalled_chunk;
    update_chunk(new_chunk,size,last_size,USE|NOMAP);

    size_t left_size = total_size - size;
    chunk_ptr left_chunk;
    if(left_size < 0x18) {
        //It's a memory fragmentation
        //so merge it with new_chunk
        left_chunk = new_chunk;
        left_chunk->size=total_size;
        update_chunk_flag(left_chunk,ismap|USE);
    } else { // it's still a chunk after cut
        left_chunk = chunk_down(new_chunk);
        update_chunk(left_chunk,left_size,new_chunk->size,ismap|NOUSE);
        install_chunk(left_chunk);
    }
    // change down chunk size
    if(!ismap) {
        chunk_ptr down = chunk_down(left_chunk);
        update_chunk(down,down->size,left_chunk->size,chunk_flag(down));
    }
    return new_chunk;
}
