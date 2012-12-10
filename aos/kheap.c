/*
	kheap.c : Memory allocation.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "kheap.h"
#include "paging.h"
#include "panic.h"

extern Uint32 end;
Uint32 placement_address = (Uint32)&end;
extern page_directory_t *kernel_directory;
heap_t *kheap=0;

Uint32 kmalloc_int( Uint32 sz, Uint8 align, Uint32 *phys )
{
	if ( kheap != 0 )
	{
		void *addr = alloc(sz,(Uint8)align,kheap);
		if ( phys != 0 )
		{
			page_t *page = get_page((Uint32)addr,0,kernel_directory);
			*phys = page->frame*0x1000+((Uint32)addr&0x0FFF);
		}
		return (Uint32)addr;
	}
	if ( align != 0 && (placement_address & 0xFFFFF000) )
	{
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}
	if ( phys )
		*phys = placement_address;
	Uint32 tmp = placement_address;
	placement_address += sz;
	return tmp;
}

void kfree(void *p)
{
	free(p,kheap);
}

Uint32 kmalloc_a( Uint32 sz )
{
	return kmalloc_int(sz,1,0);
}

Uint32 kmalloc_p( Uint32 sz, Uint32 *phys )
{
	return kmalloc_int(sz,0,phys);
}

Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys )
{
	return kmalloc_int(sz,1,phys);
}

Uint32 kmalloc( Uint32 sz )
{
	return kmalloc_int(sz,0,0);
}

static void expand( Uint32 new_size, heap_t *heap )
{
	ASSERT(new_size > heap->end_address - heap->start_address);
	if ( (new_size&0xFFFFF000) != 0 )
	{
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}
	ASSERT(heap->start_address+new_size <= heap->max_address);
	Uint32 old_size = heap->end_address-heap->start_address;
	Uint32 i = old_size;
	while ( i < new_size )
	{
		alloc_frame(get_page(heap->start_address+i,1,kernel_directory),(heap->supervisor)?1:0,(heap->readonly)?0:1);
		i += 0x1000;
	}
	heap->end_address = heap->start_address+new_size;
}

static Uint32 contract( Uint32 new_size, heap_t *heap )
{
	ASSERT(new_size < heap->end_address-heap->start_address);
	if ( new_size&0x1000 )
	{
		new_size &= 0x1000;
		new_size += 0x1000;
	}
	new_size = (new_size < HEAP_MIN_SIZE) ? HEAP_MIN_SIZE : new_size;
	Uint32 old_size = heap->end_address-heap->start_address;
	Uint32 i = old_size-0x1000;
	while ( new_size < i )
	{
		free_frame(get_page(heap->start_address+i,0,kernel_directory));
		i -= 0x1000;
	}
	heap->end_address = heap->start_address+new_size;
	return new_size;
}

static Sint32 find_smallest_hole( Uint32 size, Uint8 page_align, heap_t *heap )
{
	Uint32 iterator = 0;
	while ( iterator < heap->index.size )
	{
		header_t *header = (header_t*)lookup_ordered_array(iterator,&heap->index);
		if ( page_align > 0 )
		{
			Uint32 location = (Uint32)header;
			Sint32 offset = 0;
			if ( ((location+sizeof(header_t))&0xFFFFF000) != 0 )
				offset = 0x1000-(location+sizeof(header_t))%0x1000;
			Sint32 hole_size = (Sint32)header->size-offset;
			if ( hole_size >= (Sint32)size )
				break;
		}
		else if ( header->size >= size )
			break;
		iterator++;
	}
	if ( iterator == heap->index.size )
		return -1;
	return iterator;
}

static Sint8 header_t_less_than( void *a, void *b )
{
	return (((header_t*)a)->size < ((header_t*)b)->size) ? 1 : 0;
}

heap_t* create_heap( Uint32 start, Uint32 end_addr, Uint32 max, Uint8 supervisor, Uint8 readonly )
{
	heap_t *heap = (heap_t*)kmalloc(sizeof(heap_t));
	ASSERT(start%0x1000==0);
	ASSERT(end_addr%0x1000==0);
	heap->index = place_ordered_array((void*)start,HEAP_INDEX_SIZE,&header_t_less_than);
	start += sizeof(type_t)*HEAP_INDEX_SIZE;
	if ( (start&0xFFFFF000) != 0 )
	{
		start &= 0xFFFFF000;
		start += 0x1000;
	}
	heap->start_address = start;
	heap->end_address = end_addr;
	heap->max_address = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;
	header_t *hole = (header_t*)start;
	hole->size = end_addr-start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_ordered_array((void*)hole,&heap->index);
	return heap;
}

void *alloc( Uint32 size, Uint8 page_align, heap_t *heap )
{
	Uint32 new_size = size+sizeof(header_t)+sizeof(footer_t);
	Sint32 iterator = find_smallest_hole(new_size,page_align,heap);
	if ( iterator == -1 )
	{
		Uint32 old_length = heap->end_address-heap->start_address;
		Uint32 old_end_address = heap->end_address;
		expand(old_length+new_size,heap);
		Uint32 new_length = heap->end_address-heap->start_address;
		iterator = 0;
		Uint32 idx = -1;
		Uint32 value = 0;
		while ( iterator < heap->index.size )
		{
			Uint32 tmp = (Uint32)lookup_ordered_array(iterator,&heap->index);
			if ( tmp > value )
			{
				value = tmp;
				idx = iterator;
			}
			iterator++;
		}
		if ( idx == -1 )
		{
			header_t *header = (header_t*)old_end_address;
			header->magic = HEAP_MAGIC;
			header->size = new_length-old_length;
			header->is_hole = 1;
			footer_t *footer = (footer_t*)(old_end_address+header->size-sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
			insert_ordered_array((void*)header,&heap->index);
		}
		else
		{
			header_t *header = lookup_ordered_array(idx,&heap->index);
			header->size += new_length-old_length;
			footer_t *footer = (footer_t*)((Uint32)header+header->size-sizeof(footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}
		return alloc(size,page_align,heap);
	}
	header_t *orig_hole_header = (header_t*)lookup_ordered_array(iterator,&heap->index);
	Uint32 orig_hole_pos = (Uint32)orig_hole_header;
	Uint32 orig_hole_size = orig_hole_header->size;
	if ( (orig_hole_size-new_size) < (sizeof(header_t)+sizeof(footer_t)) )
	{
		size += orig_hole_size-new_size;
		new_size = orig_hole_size;
	}
	if ( page_align && (orig_hole_pos&0xFFFFF000) )
	{
		Uint32 new_location = orig_hole_pos+0x1000-(orig_hole_pos&0x0FFF)-sizeof(header_t);
		header_t *hole_header = (header_t*)orig_hole_pos;
		hole_header->size = 0x1000-(orig_hole_pos&0x0FFF)-sizeof(header_t);
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		footer_t *hole_footer = (footer_t*)((Uint32)new_location-sizeof(footer_t));
		hole_footer->magic = HEAP_MAGIC;
		hole_footer->header = hole_header;
		orig_hole_pos = new_location;
		orig_hole_size = orig_hole_size-hole_header->size;
	}
	else
		remove_ordered_array(iterator,&heap->index);
	header_t *block_header = (header_t*)orig_hole_pos;
	block_header->magic = HEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size = new_size;
	footer_t *block_footer = (footer_t*)(orig_hole_pos+sizeof(header_t)+size);
	block_footer->magic = HEAP_MAGIC;
	block_footer->header = block_header;
	if ( (orig_hole_size-new_size) > 0 )
	{
		header_t *hole_header = (header_t*)(orig_hole_pos+sizeof(header_t)+size+sizeof(footer_t));
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		hole_header->size = orig_hole_size-new_size;
		footer_t *hole_footer = (footer_t*)((Uint32)hole_header+orig_hole_size-new_size-sizeof(footer_t));
		if ( (Uint32)hole_footer < heap->end_address )
		{
			hole_footer->magic = HEAP_MAGIC;
			hole_footer->header = hole_header;
		}
		insert_ordered_array((void*)hole_header,&heap->index);
	}
	return (void*)((Uint32)block_header+sizeof(header_t));
}

void free( void *p, heap_t *heap )
{
	if ( p == 0 )
		return;
	header_t *header = (header_t*)((Uint32)p-sizeof(header_t));
	footer_t *footer = (footer_t*)((Uint32)header+header->size-sizeof(footer_t));
	ASSERT(header->magic==HEAP_MAGIC);
	ASSERT(footer->magic==HEAP_MAGIC);
	header->is_hole = 1;
	Uint8 do_add = 1;
	footer_t *test_footer = (footer_t*)((Uint32)header-sizeof(footer_t));
	if ( (test_footer->magic == HEAP_MAGIC) && (test_footer->header->is_hole == 1) )
	{
		Uint32 cache_size = header->size;
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		do_add = 0;
	}
	header_t *test_header = (header_t*)((Uint32)footer+sizeof(footer_t));
	if ( (test_header->magic == HEAP_MAGIC) && (test_header->is_hole) )
	{
		header->size += test_header->size;
		test_footer = (footer_t*)((Uint32)test_header+test_header->size-sizeof(footer_t));
		footer = test_footer;
		Uint32 iterator = 0;
		while ( (iterator < heap->index.size) && (lookup_ordered_array(iterator,&heap->index) != (void*)test_header) )
			iterator++;
		ASSERT(iterator < heap->index.size);
		remove_ordered_array(iterator,&heap->index);
	}
	if ( ((Uint32)footer+sizeof(footer_t)) == heap->end_address )
	{
		Uint32 old_length = heap->end_address-heap->start_address;
		Uint32 new_length = contract((Uint32)header-heap->start_address,heap);
		if ( (header->size-(old_length-new_length)) > 0 )
		{
			header->size -= old_length-new_length;
			footer = (footer_t*)((Uint32)header+header->size-sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
		}
		else
		{
			Uint32 iterator = 0;
			while ( (iterator < heap->index.size) && (lookup_ordered_array(iterator,&heap->index) != (void*)test_header) )
				iterator++;
			if ( iterator < heap->index.size )
				remove_ordered_array(iterator,&heap->index);
		}
	}
	if ( do_add )
		insert_ordered_array((void*)header,&heap->index);
}
