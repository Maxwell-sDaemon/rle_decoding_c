/*
    ONUR DEMİR 2020 18011078 VYA
*/

/*
    this implementation is work with pictures that lower than 800x800 pix(this not work with big picture in order to do that counts(frequency) cast would made size_t) and 65536(after adding histogram it is not work fine), 255 P2,P5 options.
    // im not sure it is needed now -- your int must be 4 byte o.w change typedef HUFF_TABLE_CHAR type  


    benim olusturduğum sıkıstırma formati .cmp 
    width height    sıkıstırılmıs veri
     int   int         (binary flow)
*/

#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "libs/c-linked-list/linkedlist.c"
#include "libs/c-max-heap-dynamic/heap.c"


// #include "libs/c-max-heap-static/max-heap.c"



// #include"/home/dev/Masaüstü/topcoder/pgm/libs/priority-queue/priority_queue.c"


// #define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
// #define BYTE_TO_BINARY(byte)  \
//   (byte & 0x80 ? '1' : '0'), \
//   (byte & 0x40 ? '1' : '0'), \
//   (byte & 0x20 ? '1' : '0'), \
//   (byte & 0x10 ? '1' : '0'), \
//   (byte & 0x08 ? '1' : '0'), \
//   (byte & 0x04 ? '1' : '0'), \
//   (byte & 0x02 ? '1' : '0'), \
//   (byte & 0x01 ? '1' : '0') 


// huffman decoder,encoder - structure optimization 
#define HUFFMAN_CAST_MINSZ_ENC_DEC 255
#define CHAR_STRUCT_OPTION 1 // char
#define SHORT_STRUCT_OPTION 2 // int

// formats
#define PGM_FORMAT ".pgm"
#define CMP_FORMAT ".cmp"
#define ADDON_CMP "-cmp"

#define IMAGE_PATH "./images/"

/* test data */

// P5 SAMPLES longer to take
#define EINSTEIN "einstein"
#define BANK "bank"
#define WOMEN "image1"
#define WOMEN_MAN "image2"
#define GIRL_PORTRAIT_1 "image3"
#define GIRL_PORTRAIT_2 "image4"
#define ROOF "image5"
#define TREE "image6"
#define BEADS "image7"
#define BEADS_SCATTERED "image8"
#define MOON_SURFACE "image9"
#define SATELLITE_PHOTO "image10"
#define PLANE_ALPHA "image11"
#define CLOCK_ON_WALL "image12"
#define LINE_CODE "image13"
#define VILLAGE_VIEW_FROM_HELICOPTER "image14"
#define BOOTLE_CAP "image15"
#define BLONDE_ALPHA_GIRL "image16"
#define BABOON "image17"
#define GIRL_LOOK_BACK "image18"
#define JET_F16_WINTER "image19"
#define PEPPERS "image21"
#define WOMAN_MAN_2 "image22"
#define SATELLITE_ALPHA_PHOTO "image23"
#define BRIDGE_PHOTO "image24"
#define NATIVE_MAN "image25"
#define SATELLITE_BLACKED_PHOTO "image26"
#define CRODS "crods"

// P2 SAMPLES
#define MONA_LISA_FAST "mona_lisa.ascii" // 250x360 a little bit fast
#define COINS_FAST "coins.ascii"
#define GLASSWARE_NOISY_FAST "glassware_noisy.ascii" //320x428
#define FRACTAL_TREE "fractal_tree.ascii"

// FAST SAMPLE
#define FEEP_TEST_FASTEST "feep.ascii" // 24 x 7 image for test purpose

#define PROCEDING printf("#############################################################################\n");

#define FALSE 0
#define TRUE 1

#define _1_BYTE_MAX_VALUE 255
#define _2_BYTE_MAX_VALUE 65536

#define _1BYTE 1
#define _2BYTE 2
#define BINARY_ONE 0b1

#define CHUNK 50

#define SKIP fgetc(fp);
#define NEWLINE fputc('\n', fp);

#define LEAF_NODE '0'
#define PARENT_NODE '1'

#define EOL NULL

#define FREE_BUFFER 0

#define WORD_LENGTH_BIT 8

#define YES 'y'
#define YES_ 'Y'

#define NO 'n'
#define NO_ 'N'

#define UNALLOWED_VALUE 256

typedef unsigned char CLASSIC_IMAGE;
typedef unsigned short ULTRA_RES_IMAGE;
typedef unsigned long size_t;
typedef unsigned int boolean;
// this depen on c implementation
// typedef int HUFFMAN_TABLE_CHAR;

// it is hard to interpret binary flow how much 1 or 0 is in also 0 is disapperar because of there is 0 in start
// ---- this holds binary_representation - number length of bit stream;
/*
    high 1byte nibble               low 1byte nibble
         00000111                       00000011
        binary rep                   binary rep length in this case it s 3    
*/
typedef unsigned short binary_cast;
typedef unsigned char WORD;

typedef struct canvas {
    int width;
    int height;
    int color_bandwidth; // rendering option (255) (65536) ...
    char version ; // 5 for binary(P5) , plain 2 (P2)  
    int * image; 
} Canvas;

typedef struct rle {
    List * counts_lst;
    List * pixies_lst;
    // bu min_heap olacak ::refactor, TODO
    heap_t * max_heap; // for huffman optimzation . this hold counts as max heap 
    int length; // two are same and pairwise. length is length of them
    
    int width; // for uncomressed_image 
    int height;
    int color_width;

} RLE_PAIRS ;

// hufman tree sıkıştırma oranını artıracak
// bir sonraki byte a yuvarlanarak verilebilir
// ya da bastan verilebilir. bastan verilenler count dizisinde tutulur.
// sonrakiler ise eslenikleri olur.
typedef struct huffman_node {
    char state; // 0 state leaf node , 1 state parent node of 
    struct huffman_node *right_node;
    struct huffman_node *left_node;
    int frequency;
} HuffmanNode ;

/*
    optimization hash map implementation ::hashmap , HashMap, TODO, todo
*/

// // assume 65536 , 255 is values picture size is 800x800 640.000
// typedef struct huffman_decoder_node {
//     HUFFMAN_TABLE_CHAR key; // must be 4 byte from above
//     char value; // no more than log255 log65536 ~ 10 
//     struct huffman_decoder_node *next;
// } Huffman_Decoder;

// typedef struct huffman_encoder_node {
//     char key;
//     HUFFMAN_TABLE_CHAR value;
//     struct huffman_encoder_node *next;
// } Huffman_Encoder;


typedef struct huffman_tables {

    List* frequency_lst;
    List* binary_lst;

    // Huffman_Encoder * huffman_encoder;
    // Huffman_Decoder * huffman_decoder;
    
    // ilerde eklenebilir.
    // char sz_count_sz; // give us dynamic casting 1Byte 1 2 BYTE 2
    // char sz_huffman_code_sz;
} Huffman_Table;

typedef struct histogram {
    size_t * color_count_arr;
    size_t color_count;
    size_t size;
} Histogram;


RLE_PAIRS* make_rle(Canvas* canvas)
{
    int i ;

    RLE_PAIRS * rle_pairs = (RLE_PAIRS*) malloc( sizeof(RLE_PAIRS) );
    rle_pairs->length = 0;
    rle_pairs->counts_lst = makelist();
    rle_pairs->pixies_lst = makelist();

    int value2write = canvas->image[0];
    int count = 1;

    // heap initialization for max-heap malloc is not allowed.
    // Heap tmp = {0,{0}}/*(Heap*) malloc(sizeof(Heap))*/ ;
    // Heap* use_count_heap = &tmp;

    size_t element_sz = canvas->height * canvas->width; 

    heap_t * seq_color_count_heap = heap_create(element_sz, free);

    for ( i = 1 ; i <  element_sz  /*10*/; i++)
    {
        
        if(value2write == canvas->image[i])
        {
            count++;
            if (i == element_sz - 1) // from tests all are same pixel
            {
                add(count, rle_pairs->counts_lst);
                // insert(use_count_heap, count); //static heap
                HuffmanNode* huffman_node = (HuffmanNode*) malloc(sizeof(HuffmanNode));

                huffman_node->frequency = count;
                huffman_node->left_node = huffman_node->right_node = NULL;
                huffman_node->state = '0'; // this states that node is leaf node 

                // give negative makes min heap
                heap_insert(seq_color_count_heap, -count, huffman_node); // key count value is huffman node . Huffman algorithm
                add(value2write, rle_pairs->pixies_lst);
                rle_pairs->length++;
                count = 1;
                value2write = canvas->image[i];
            }

        }
        else
        {
            add(count, rle_pairs->counts_lst);
            // insert(use_count_heap, count); //static heap
            HuffmanNode* huffman_node = (HuffmanNode*) malloc(sizeof(HuffmanNode));

            huffman_node->frequency = count;
            huffman_node->left_node = huffman_node->right_node = NULL;
            huffman_node->state = '0'; // this states that node is leaf node 

            heap_insert(seq_color_count_heap, -count, huffman_node); // key count value is huffman node . Huffman algorithm
            add(value2write, rle_pairs->pixies_lst);
            rle_pairs->length++;
            count = 1;
            value2write = canvas->image[i];

        }

    }


    PROCEDING

    display(rle_pairs->counts_lst);
    display(rle_pairs->pixies_lst);
    //print(use_count_heap);

    // rle_pairs->max_heap = use_count_heap;
    rle_pairs->max_heap = seq_color_count_heap;

    printf("ardışıl veri uzunluğu : %d \n", rle_pairs->length);
    printf("resimdeki veri uzunluğu : %d \n", canvas->height * canvas->height );

    float gain_percentile = 2 * rle_pairs->length/(float) (canvas->height * canvas->width);
    printf("elde edilebilecek verim(kat olarak) %%%f\n", 100 * 1/gain_percentile);

    PROCEDING

    return rle_pairs;
}


void println(int * line, int length)
{
    int i;
    for( i = 0 ; i < length ; i++ )
    {
        printf("%d ", line[i]);
    }
    printf("\n");
}

int * read_image_1Byte(  FILE *fp, Canvas* c )
{
    int * image = (int*) malloc (sizeof(int) * c->height * c->width);
    int i, j;

    CLASSIC_IMAGE pixel; // each variable block in block (0-255) 

    for ( i = 0; i < c->height; i++ )
    {
        // if(i == 509) { // for debug line 
        for( j = 0 ; j < c->width ; j++) 
        {
            int dumb = i; // fread issue   
            fread(&pixel, sizeof(_1BYTE), 1, fp);
            i = dumb;
            
            //printf("%ld\n", sizeof(int));
            // printf( BYTE_TO_BINARY_PATTERN ""  , BYTE_TO_BINARY(fi));

 
            image[i*c->width + j] = 0 | pixel; // extend with 0
            // printf("%d ", image[i*c.width + j]);
            //  break;


        }

    }

    fclose(fp);

    return image;
}

int * read_image_2Byte(FILE *fp, Canvas* c)
{
    int * image = (int*) malloc (sizeof(int) * c->height * c->width);
    int i, j;

    ULTRA_RES_IMAGE pixel; // each variable block in block (0-255) 

    for ( i = 0; i < c->height; i++ )
    {
        // if(i == 509) { // for debug line 
        for( j = 0 ; j < c->width ; j++) 
        {
            int dumb = i; // fread issue   
            fread(&pixel, sizeof(_1BYTE), 1, fp);
            i = dumb;
            
            //printf("%ld\n", sizeof(int));
            // printf( BYTE_TO_BINARY_PATTERN ""  , BYTE_TO_BINARY(fi));

 
            image[i*c->width + j] = 0 | pixel; // extend with 0
            // printf("%d ", image[i*c.width + j]);
            //  break;
            
    
        }
            
    }
    
    fclose(fp);

    return image;
}

Canvas* print_PGM(char* file)
{
    FILE* fp;

    int* image;
    int i, j, read_block_sz; // binary reading buffer(block) size
    Canvas* canvas = (Canvas*) malloc( sizeof(Canvas*) );

    char file_[CHUNK], file_path[CHUNK];

    strcpy(file_, file) ;
    strcpy(file_path, IMAGE_PATH);

    strcat(file_path, file);

    fp = fopen( file_path, "rb" );

    if(fp == NULL)
    {
        printf("Error! Check filename ");
        exit(1);
    }

    printf("file is reading ...");

    char format = fgetc(fp);
    char version = fgetc(fp);
    printf("format : %c%c\n", format, version); 

    canvas->version = version;    
    
    SKIP

    printf("format : %c%c\n", format, version); 
    fscanf(fp, "%d %d", &canvas->width, &canvas->height );
    printf("%d %d\n", canvas->width, canvas->height);

    SKIP

    int color_bandwidth ;

    fscanf(fp, "%d", &color_bandwidth);
    printf("rendering width : %d \n", color_bandwidth);
    canvas->color_bandwidth = color_bandwidth;

    SKIP

    PROCEDING

    // printf("\n%d  %d\n", canvas->version,'2');
    if(canvas->version == '2') 
    {
        printf("old plain ascii format(P2) extracting...\n");
        int m_tmp_char;
        
        canvas->image = (int*) malloc( sizeof(int) * canvas->height * canvas->width );

        for(i = 0 ; i < canvas->height ; i++)
        {
            for ( j = 0; j < canvas->width; j++)
            {
                fscanf( fp, "%d", &m_tmp_char );    

                canvas->image[ i * canvas->width + j ] = m_tmp_char;
            }

        }

        printf("image flowing ... \n");

        for ( i = 0; i < 100; i++ )
        {
            printf("%d ", canvas->image[i]);
        }
        printf("\nsample flow finished\n");


        return canvas;

    }
    /*elif ... if any other version appears :: new version, new p , new P*/
    else 
    {
        /*
        dynamic casting
        */
        if( color_bandwidth > _2_BYTE_MAX_VALUE ) {
            printf("this image format is wrong . Are you sure it is pgm format ?\n");
            printf("for more info please look at : http://netpbm.sourceforge.net/doc/pgm.html\n");
            return NULL;
        }
        else if( color_bandwidth > _1_BYTE_MAX_VALUE )
        {
            // printf("oh boy time to see einstein \n\n");
            canvas->image = read_image_2Byte(fp, canvas);
        }
        else
        {
            // printf("oh boy time to see einstein \n\n");
            canvas->image = read_image_1Byte(fp, canvas);        
        }

        printf("image flowing ... \n");

        for ( i = 0; i < 100; i++ )
        {
            printf("%d ", canvas->image[i]);
        }
        printf("\nsample flow finished\n");

    }

    return canvas;
}

binary_cast _find_huffman_binary_value(Huffman_Table* ht, int frequency )
{
    Node* frequency_lst_itr = ht->frequency_lst->head;
    Node* binary_lst_itr = ht->binary_lst->head;

    while(frequency_lst_itr != EOL && binary_lst_itr != EOL )
    {
        if(frequency_lst_itr->data == frequency)
            return binary_lst_itr->data; // :: TODO refactor list as type of 2 byte option

        frequency_lst_itr = frequency_lst_itr->next;
        binary_lst_itr = binary_lst_itr->next;
    }
    printf("!!!!!!!!!!!!!!!!!!DEBUG1!!!!!!!!!!!!!!!!!!!!\n");
    return -1;// ERROR
}

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
        printf(" ");
    }
    printf(" ");
    //puts("");
}

// başta hatalı yaptıgım için
WORD reverseBits( WORD num)
{
    WORD  NO_OF_BITS = sizeof(num) * 8;
    WORD reverse_num = 0;
    int i;
    for (i = 0; i < NO_OF_BITS; i++)
    {
        if((num & (1 << i)))
           reverse_num |= 1 << ((NO_OF_BITS - 1) - i);  
   }
    return reverse_num;
}

/* 
    bu fonksiyonun stack trace ini deftere yazdım.
*/ 
void write_buffer( FILE *fp, binary_cast binary, WORD * buffer, WORD * buffer2, WORD * cursor, WORD * buffer2_sz ){

    // static WORD cursor = 0; // this hold how much filled word example cursor / 8 
    // static WORD buffer2_sz = 0;

    static int i = 0; // for debug purpose release some flow.

    WORD binary2write = binary >> 8;
    WORD binary_length =  binary;


    // if buffer length eight and is_buffer_next_active
    // 
    // if( *is_buffer_next_active )
    // {
    //     *buffer = *buffer2;
    //     *buffer2 = FREE_BUFFER;
    //     is_buffer_next_active = NEXT_BUFFER_INACTIVE;
    //     *cursor = *buffer2_sz;
    // }

    if( ( *cursor + binary_length ) > WORD_LENGTH_BIT ) {

        // this length bit add to first buffer -- WORD_LENGTH_BIT - cursor;
        //  place_i_filled WORD_LENGTH_BIT - cursor :: TODO refactor here find good variable name
        
        /*
                *buffer = ( *buffer >> WORD_LENGTH_BIT - *cursor ) |  binary2write << ( *cursor );
        *buffer2 = ( binary2write >> WORD_LENGTH_BIT - *cursor );

        
        */

        *buffer = *buffer | (binary2write << (WORD_LENGTH_BIT - binary_length) ) >> ( *cursor ); //000
        
        *buffer2_sz = binary_length - (WORD_LENGTH_BIT - (*cursor));
        *buffer2 = ( binary2write << WORD_LENGTH_BIT - *buffer2_sz);

        // *buffer2_sz = binary_length - (WORD_LENGTH_BIT - (*cursor));

        //*buffer = reverseBits(*buffer);
        // ters yazdırdım biti ters cevirmem gerekiyor
        
        fputc(*buffer, fp);

        if(i < 50) {
            printBits(sizeof(WORD), buffer);
        }

        // buffer loading 7 bit to 0 bit we replace our buffer 2  :: TODO refactor buffer issue
        // buffer 2 in order so 
        *buffer = *buffer2;
        *cursor = *buffer2_sz;
        *buffer2 = FREE_BUFFER;
        *buffer2_sz = 0;
        // this length bit added sec buffer -- cursor + binary_length - WORD_LENGTH_BIT;

    } else {

        *buffer = ( *buffer ) |  binary2write << ( WORD_LENGTH_BIT - *cursor - binary_length );
        *cursor += binary_length;

    }
    i++;

}

binary_cast _image_pixel2binary_cast(int pixel)
{
    return (binary_cast) pixel << 8 | 8;
}

void write_rle_pair2file_h(char* filename, RLE_PAIRS * rle_pair, Huffman_Table* ht, int img_width, int img_height, int color_width ) // :: TODO refactor rle_pair renamed it 
{
    int i;
    
    FILE *fp;

    printf("compression file creating...\n\n");

    fp = fopen( filename, "wb+");
    printf("writing width and height of image ...\n ");
    fprintf(fp, "%d %d %d", img_width, img_height, color_width);


    WORD _temp;
    WORD _8_bit_insertion;

    Node* counts_lst_itr = rle_pair->counts_lst->head;
    Node* image_pixels_itr = rle_pair->pixies_lst->head; 


    WORD buffer = FREE_BUFFER;
    WORD buffer_next = FREE_BUFFER;


    // in order to use in static variable in write buffer
    WORD cursor = 0, buffer2_sz = 0;

    printf("sample stream flowing ... \n");
    for( i = 0 ; i < rle_pair->length ; i++ )
    {

        // if( rle_pair->length -1 == i ) {
        //     printf(" ");
        // }

        // put file pair of binary 
        binary_cast binary = _find_huffman_binary_value( ht, counts_lst_itr->data );
        write_buffer( fp, binary, &buffer, &buffer_next, &cursor, &buffer2_sz );

        binary = _image_pixel2binary_cast(image_pixels_itr->data);
        write_buffer( fp, binary, &buffer, &buffer_next, &cursor, &buffer2_sz );

        counts_lst_itr = counts_lst_itr->next;
        image_pixels_itr = image_pixels_itr->next;

    }

    // if there is a buffer to insert , add buffer to comp file
    if (cursor > 0) {
        fputc(buffer, fp);
    }

    printf("\nsample stream flow finished.\n");

    fclose(fp);
    printf("\nfile is closed.\n");

}

HuffmanNode * create_huffman_tree(heap_t* max_heap)
{

    HuffmanNode *root,  *m_max1_node, *m_max2_node, *new_huffman_node;
    while( heap_get_size(max_heap) > 1 )
    {

        m_max1_node = (HuffmanNode*) heap_get_max(max_heap);
        heap_destroy_max_without_free(max_heap);

        m_max2_node = (HuffmanNode*) heap_get_max(max_heap);
        heap_destroy_max_without_free(max_heap);

        new_huffman_node = (HuffmanNode*) malloc(sizeof(HuffmanNode));
        new_huffman_node->frequency = m_max1_node->frequency + m_max2_node->frequency;
        new_huffman_node->left_node = m_max1_node;
        new_huffman_node->right_node = m_max2_node;
        new_huffman_node->state = '1';

        heap_insert2(max_heap, -new_huffman_node->frequency, new_huffman_node);
    }


    if( heap_get_size(max_heap) == 0 ) {
        root = new_huffman_node;
    }
    else {
        m_max1_node = (HuffmanNode*) heap_get_max(max_heap);
        heap_destroy_max_without_free(max_heap);

        root = m_max1_node;


        // root = (HuffmanNode*) malloc ( sizeof(HuffmanNode) );

        // root->right_node = m_max1_node;
        // root->left_node = m_max2_node;
        // root->state = '1';
        // root->frequency = 0;

        // if ( m_max1_node )
        // {
        //     root->frequency += m_max1_node->frequency;
        // }

        // if ( m_max2_node )
        // {
        //     root->frequency += m_max2_node->frequency;
        // }
    }

    return root;
}

// think about how can optimize this overhead :: TODO optimization function overhead (static not allowed)
void _create_huffman_table( Huffman_Table* ht, HuffmanNode* huffman_node, binary_cast binary, unsigned char length_bit ) 
{
    if(huffman_node->state == LEAF_NODE) {
        binary = binary << 8; // open space for binary size value
        binary = binary | length_bit;
        add(huffman_node->frequency, ht->frequency_lst);
        add(binary, ht->binary_lst);
        return;
    }

    // preorder traversal
    int tmp = ++length_bit;
    _create_huffman_table( ht, huffman_node->right_node, ( binary << 1 ) | 1 , tmp ) ;

    _create_huffman_table( ht, huffman_node->left_node, binary << 1 ,  tmp);
}

void create_huffman_table( Huffman_Table* ht, HuffmanNode* huffman_tree )
{   

    binary_cast one = 1, zero = 0;
    // preorder traversal
    _create_huffman_table( ht, huffman_tree->right_node, one, 1 );
    
    
    _create_huffman_table( ht, huffman_tree->left_node, zero, 1 );

}

Huffman_Table* make_huffman_table()
{
    Huffman_Table* ht = (Huffman_Table*) malloc(sizeof(Huffman_Table));
    ht->frequency_lst = makelist();
    ht->binary_lst = makelist();

    return ht;
}



void display_huffman_table(Huffman_Table * ht)
{
    Node* binary_itr = ht->binary_lst->head;
    Node* frequency_itr = ht->frequency_lst->head;

    printf("\n\t[0-7] bit size other bits [7-x] are encoded value.\n");
    while( binary_itr != EOL && frequency_itr != EOL )
    {
        printf( "key:%d  value: ", frequency_itr->data );
        printBits( sizeof(binary_itr->data), binary_itr );
        printf("\n");
        binary_itr = binary_itr->next;
        frequency_itr = frequency_itr->next;
    }

}

WORD first_bit_of_stream(WORD m_word )
{
    return (m_word ) >> 7;    
}

// last byte is not complete 8 bit
// can be some zeros eof 

boolean _read_pixel_(FILE* fp, RLE_PAIRS * rle_pairs, WORD * m_word , WORD * m_word_length, WORD * buffer, WORD * buffer_length, WORD *cntrl_pix, boolean* is_control_active)
{
    // there is need to use buffer length

    // if buffer length is greater than 0
    if(*buffer_length > 0)
    {

        // :: TODO LOOP unbranching
        
        // fill word with buffer
        *m_word = *m_word | *buffer >> *m_word_length;
        // increase word length
        WORD free_space_first = WORD_LENGTH_BIT - *m_word_length;
       
        if (*buffer_length > free_space_first)
        {
            *buffer_length -= free_space_first;
            *m_word_length = WORD_LENGTH_BIT;
            *buffer = *buffer << free_space_first;
        }
        else
        {
            *m_word_length += *buffer_length;
            *buffer_length = 0;
            *buffer = FREE_BUFFER;
        }

        // if ((*m_word_length + *buffer_length)  > WORD_LENGTH_BIT)
        // {
        //     *m_word_length = WORD_LENGTH_BIT;    
        // }
        // else 
        // {
        //     *m_word_length = (*m_word_length + *buffer_length);
        // }

        // free buffer

        // word length is 8 is complete
        if(*m_word_length == 8) {
            // write - copy pixel
            
            //  check if it is 0-255 and sequentially stable.
            // unsigned char
            if(*m_word > 255){
                printf("this pixel color is not allowed.\n");
                return FALSE; // her okuma 0-255 word kadar bu satır gereksiz.
            }
            if(*is_control_active == TRUE && *cntrl_pix == *m_word) {
                printf("this pixel (%d) is repeated.\n", *m_word);
                return FALSE;
            }
            *cntrl_pix = *m_word;
            *is_control_active = TRUE;
            add(*m_word, rle_pairs->pixies_lst);

            // free m_word
            *m_word = FREE_BUFFER;

            // copy buffer to m_word
            *m_word = *buffer;
            *m_word_length = *buffer_length;

            *buffer = FREE_BUFFER; // TODO :: REFACTOR refactor them just hold the debugging issue

            // read buffer
            *buffer = fgetc(fp);
            *buffer_length = WORD_LENGTH_BIT;
            // if eof
            if( *buffer == EOF )
            {
                printf("error length size more less than image size.\n");
                return FALSE;
            }
            // fill m_word with buffer
            *m_word =  *m_word | *buffer >> *m_word_length;

            *buffer = * buffer << ( WORD_LENGTH_BIT - *m_word_length); 
            // decrease buffer_length
            *buffer_length -= ( WORD_LENGTH_BIT - *m_word_length );

            // if buffer length 0 add new bufer for each one 


            // word length 8
            *m_word_length = WORD_LENGTH_BIT;
        }
        else
        {
            // read buffer 
            *buffer = fgetc(fp);
            *buffer_length = WORD_LENGTH_BIT; // dont need

            // fill word with buffer
            // TODO :: refactor this approach take lots of place, give it function
            free_space_first = WORD_LENGTH_BIT- *m_word_length;
            *m_word = *m_word | *buffer >> (*m_word_length );
            
            // word length 8
            *m_word_length = WORD_LENGTH_BIT;

            // update buffer left hand side filled
            *buffer = *buffer << free_space_first;

            // decrease buffer length
            *buffer_length -= free_space_first;

            // write - copy m_word to pixel

            //  check if it is 0-255 and sequentially stable.
            if(*m_word > 255){
                printf("this pixel color is not allowed.\n");
                return FALSE; // her okuma 0-255 word kadar bu satır gereksiz.
            }
            if(*is_control_active == TRUE && *cntrl_pix == *m_word) {
                printf("this pixel (%d) is repeated.\n", *m_word);
                return FALSE;
            }
            *is_control_active = TRUE;
            *cntrl_pix = *m_word;
            

            add( *m_word, rle_pairs->pixies_lst );
            
            // free m_word
            *m_word = FREE_BUFFER;

            // word_length = 0
            

            // write m_word to buffer
            *m_word = *buffer;
 
            *m_word_length = *buffer_length;
            // TODO :: think about adresses for efficiency
            // free buffer
            *buffer = FREE_BUFFER;

            // buffer_length = 0
            *buffer_length = 0;

            // read buffer
            *buffer = fgetc(fp);// garantilemek için
            *buffer_length = WORD_LENGTH_BIT;

            if ( *buffer == EOF ){ 
                 printf("length size less than image size.\n");
                return FALSE;
            }
            *m_word = *m_word | *buffer >> (*m_word_length);

            *buffer = *buffer << (WORD_LENGTH_BIT - *m_word_length); 
            // decrease buffer length
            *buffer_length -= (WORD_LENGTH_BIT - *m_word_length);
            // word length 8
            *m_word_length = WORD_LENGTH_BIT;

        }

    }

    // buffer length is 0
    else {

        // buffer read 1 word 
        *buffer = fgetc(fp);
        *buffer_length = WORD_LENGTH_BIT;
        // fill m_word that makes it pixel 
        *m_word = *m_word | (*buffer >> *m_word_length);
        
        *buffer = ( *buffer << WORD_LENGTH_BIT - *m_word_length );
        *buffer_length -= (WORD_LENGTH_BIT - *m_word_length) ;

        // copy pixels new variable
        // write rle_pair
        
        //  check if it is 0-255 and sequentially stable.
        if(*m_word > 255){
            printf("this pixel color is not allowed.\n");
            return FALSE; // her okuma 0-255 word kadar bu satır gereksiz.
        }
        if(*is_control_active == TRUE && *cntrl_pix == *m_word) {
            printf("this pixel (%d) is repeated.\n", *m_word);
            return FALSE;
        }
        *is_control_active = TRUE;
        *cntrl_pix = *m_word;
        

        add( *m_word, rle_pairs->pixies_lst );

        // freed m_word
        *m_word = FREE_BUFFER;
        *m_word_length = 0;

        //  write buffer to m_word
        *m_word = *buffer;
        *m_word_length = *buffer_length;

        *buffer = fgetc(fp);
        *buffer_length = WORD_LENGTH_BIT;

        if( *buffer == EOF )
        {
            printf("length size less than image size.\n");
            return FALSE;
        }

        *m_word = *m_word | (*buffer >> *m_word_length);
        
        *buffer = *buffer << (WORD_LENGTH_BIT- *m_word_length);
        *buffer_length = *buffer_length - (WORD_LENGTH_BIT - *m_word_length);
        *m_word_length = WORD_LENGTH_BIT;
    }

    return TRUE;
}


RLE_PAIRS * _open_compressed_file(FILE* fp, HuffmanNode* h_root , int width, int height , int color_width) {

    int i = width * height;

    RLE_PAIRS * rle_pairs = (RLE_PAIRS*) malloc( sizeof (RLE_PAIRS) );
    rle_pairs->counts_lst = makelist();
    rle_pairs->pixies_lst = makelist();
    rle_pairs->length = 0;
    rle_pairs->color_width = color_width;

    printf("successfull initialization.\n");

    int pair;

    WORD m_word = fgetc(fp), m_word_length = WORD_LENGTH_BIT, buffer = FREE_BUFFER, buffer_length = 0, cntrl_pix = 0;

    // after reading first pixel it s makes active
    boolean is_cntrl_active = FALSE;


    // so dumb slow implenetation :: TODO implementation issue , SLOW, ERR -- implement some mapping technique new ds or loop unbranching switch state use 
    // boolean is_found = FALSE;

    HuffmanNode *count_itr ;
    int huffman_passing_bit_count = 0, frequency = 0/*, tmp_img_pix = 0, img_pix = -1*/;

    // hufman decoding
    // i image size
    while( i > 0 )
    {
        count_itr = h_root;

        // take last bit
        while ( count_itr->state == '1' ) {

            if( first_bit_of_stream(m_word)  == BINARY_ONE )
            {
                if(count_itr->right_node == NULL  /*&& pair_length != pair_length - 1 */) {
                    printf("Access key denied ! \n Huffman decoding unsuccess. \n");
                }
                count_itr = count_itr->right_node;
            }

            else
            {
                if(count_itr->left_node == NULL  /*&& pair_length != pair_length - 1 */ ) { // 
                    printf("Access key denied ! \n Huffman decoding unsuccess. \n");
                }
                count_itr = count_itr->left_node;
            }

            huffman_passing_bit_count++;
            m_word = m_word << 1;
        }


        frequency = count_itr->frequency;

        rle_pairs->length++;
        add(frequency, rle_pairs->counts_lst);

        i -= frequency;

        // huffman okunan bit kadar kaydırma    
        //m_word = m_word << ( huffman_passing_bit_count );
        m_word_length -= huffman_passing_bit_count;
        huffman_passing_bit_count = 0;
        boolean check =_read_pixel_( fp ,rle_pairs, &m_word, &m_word_length, &buffer, &buffer_length, &cntrl_pix, &is_cntrl_active );

        if (!check)
        {
            printf("returning...\n");
            return NULL;
        }
        

    }

    // TODO burada uc durumlra baılmalı. !!!
    char a;
    if( (a = fgetc(fp)) != EOF ) {
        printf("After reading compressed file.\n Size mismatch occured.\n ");
        printf("your compressed file size is more greater than your file size.\n");
    }

    fclose(fp);
    // printf("!!!!!!!!!!!!!!!!!!DEBUG2!!!!!!!!!!!!!!!!!!!!\n");
    rle_pairs->height = height; // overflow!!
    rle_pairs->width = width;
    return rle_pairs;
}

// huffman tree kullanmak daha avantajlı ama tablo kullanmayı tercih ettim sonradan hash table eklenirse diye. 
RLE_PAIRS* open_compressed_file_with_img_sz( char* filename , HuffmanNode *  h_root, int img_width, int img_height, int img_color_width )
{

    FILE * fp;
    fp = fopen(filename, "rb");

    if(fp == NULL) 
    {
        printf("Error! check filename.\n");
        exit(1);
    }

    int width , height, color_width;

    fscanf(fp, "%d %d %d", &width , &height ,&color_width);

    if (img_width == width && height == img_height && img_color_width == color_width)
    {
        return _open_compressed_file(fp, h_root, width , height, color_width);
    }
    else
    {
        printf("image size is incompatible.\n");
        return NULL;
    }
    printf("!!!!!!!!!!!!!!!!!!DEBUG3!!!!!!!!!!!!!!!!!!!!\n");

    return NULL;

}


void display_uncompressed_image_unchecked( RLE_PAIRS * unchecked_image )
{
    printf("sample checked image flowing.. \n");

    // daha iyi görmek için yorum satırındaki kullanılabilir.
    display(unchecked_image->counts_lst);
    display(unchecked_image->pixies_lst);

    // Node * img_counts_itr = unchecked_image->counts_lst->head;
    // Node * img_pixels_itr = unchecked_image->pixies_lst->head;

    // while (img_counts_itr != EOL && img_pixels_itr != EOL )
    // {
    //     printf("%d %d", img_counts_itr, img_pixels_itr);

    //     img_counts_itr = img_counts_itr->next;
    //     img_pixels_itr = img_pixels_itr->next;
    // }
    


    printf("sample checked image flowing finished\n");
}


boolean check_legal_0_255_pix(RLE_PAIRS * image_pairs) {

    Node * pixels_itr = image_pairs->pixies_lst->head;

    while ( pixels_itr != NULL)
    {
        if( pixels_itr->data > 256 && pixels_itr->data < 0)
        {
            return FALSE;
        }
        pixels_itr = pixels_itr->next;
    }

    return TRUE;
}

boolean check_order_rle( RLE_PAIRS * rle_pairs )
{
    Node * pix_itr = rle_pairs->pixies_lst->head;

    int tmp = pix_itr->data;

    while(pix_itr != NULL)
    {
        if(pix_itr->next != NULL && pix_itr->data == tmp ) 
        {
            return FALSE;
        }
        pix_itr = pix_itr->next;
    }

    return FALSE;
}

// sıkıstırılmıs formatı pgm olarak yazar.
void write_pgm2_file(RLE_PAIRS* rle_pairs, char* filename)
{
    FILE *fp;
    
    int height = rle_pairs->height;
    int width = rle_pairs->width;

    fp = fopen(filename , "w");

    fputc('P',fp);
    putchar('P');

    fputc('2',fp);
    putchar('2');

    NEWLINE
    putchar('\n');

    fprintf(fp, "%d %d", width, height);
    printf("%d %d", width, height);

    NEWLINE
    putchar('\n');

    fprintf(fp, "%d", rle_pairs->color_width);
    printf("%d", rle_pairs->color_width);

    NEWLINE
    putchar('\n');


    // write all 
    size_t i, j, k = 0;

    Node* count_lst_itr = rle_pairs->counts_lst->head;
    Node* pixels_lst_itr = rle_pairs->pixies_lst->head;

    for( i = 0; i < rle_pairs->length ; i++ )
    {
        for ( j = 0; j < count_lst_itr->data; j++)
        {
            fprintf(fp, "%d", pixels_lst_itr->data);
            printf("%d", pixels_lst_itr->data);
            k++;
            if( (k % width) == 0 )
            {
                NEWLINE
                putchar('\n');
                continue;
            }
            fputc(' ', fp);
            putchar(' ');
            
        }
        
        
        count_lst_itr = count_lst_itr->next;
        pixels_lst_itr = pixels_lst_itr->next;

    }

    fclose(fp);
}

void change_color( RLE_PAIRS * uncompressed_image, int color2chg, int color_ )
{
    size_t i;
    
    Node* counts_itr = uncompressed_image->counts_lst->head;
    Node* pixels_itr = uncompressed_image->pixies_lst->head;
    boolean found = 0;

    for( i = 0 ; i < uncompressed_image->length ; i++ )
    {
        if( pixels_itr->data == color2chg )
        {
            found = TRUE;
            pixels_itr->data = color_;
        }
        pixels_itr = pixels_itr->next;
    }
    if (!found)
    {
        printf("this color is not found.\n");
    }
    
    
}

Histogram* create_histogram(RLE_PAIRS* u_image)
{
    Histogram* hist = (Histogram*) malloc(sizeof(Histogram));
    hist->color_count_arr = (size_t*) calloc(sizeof(size_t) , u_image->color_width );
    hist->color_count = 0;
    size_t i ;
    hist->size = u_image->color_width;

    Node* counts_itr = u_image->counts_lst->head;
    Node* pixels_itr = u_image->pixies_lst->head;

    for ( i = 0 ; i < u_image->length ; i++ )
    {
        if(hist->color_count_arr[pixels_itr->data] == 0){
            hist->color_count++;
        }
        hist->color_count_arr[pixels_itr->data] += counts_itr->data; 

        counts_itr = counts_itr->next;
        pixels_itr = pixels_itr->next;
    }
    
    return hist;
}

void display_histogram(Histogram* hist)
{
    printf("total color is %ld .\n", hist->color_count );
    size_t i = 0;

    size_t* hist_arr = hist->color_count_arr;
    for( i = 0 ; i < hist->size ; i++)
    {
        if(hist_arr[i] > 0) {
            printf("%ld: %ld |", i, hist_arr[i]);
        }
    }
    printf("\n");
}

void change_row_colomn_2pixel(RLE_PAIRS* rle_pairs, int row, int column, int pixel)
{
    int width = rle_pairs->width;
    int height = rle_pairs->height;

    Node* counts_itr  = rle_pairs->counts_lst->head;
    Node* pixels_itr = rle_pairs->pixies_lst->head;

    if(pixel > rle_pairs->color_width )
    {
        printf("this pixel is greater than colorwidth %d\n", rle_pairs->color_width);
        return;
    }

    int i = row * width + column;

    if( i >= width*height || row >= height || column >= width || column < 0 || row < 0 ) 
    {
        printf("Err: Size vulnature. \n");
        return;
    }


    Node * tmp_c ,* tmp_p , *node_c, *node_p;
    
    node_c = (Node*) malloc( sizeof(Node));
    node_p = (Node*) malloc( sizeof(Node));   

    // 0,0 state
    if ( i < counts_itr->data ) {
        node_c->data = 1;
        node_c->next = counts_itr;
        rle_pairs->counts_lst->head = node_c;
        counts_itr->data--;

        node_p->data = pixel;
        node_p->next = pixels_itr;
        rle_pairs->pixies_lst->head = node_p;
        rle_pairs->length++;
        return;
    }
    
    // do while
    while( i > 0 )
    {
        i -= counts_itr->data;

        tmp_c = counts_itr;
        tmp_p = pixels_itr;
        counts_itr= counts_itr->next; 
        pixels_itr = pixels_itr->next;
    }

    if( i == 0 )
    {
        node_c->data = 1;
        node_c->next = counts_itr;
        tmp_c->next = node_c;
        counts_itr->data--;

        node_p->data = pixel;
        node_p->next = pixels_itr;
        tmp_p->next = node_p;
        rle_pairs->length++;
        return;
    }

    tmp_c->data += i;

    node_p->data = pixel;
    node_c->data = 1;


    Node* after_node_c = (Node*) malloc(sizeof(Node));
    after_node_c->data = -i  - 1; 

    tmp_c->next = node_c;
    node_c->next = after_node_c;
    after_node_c->next = counts_itr;

    Node* after_node_p = (Node*) malloc(sizeof(Node));
    after_node_p->data = tmp_p->data;

    tmp_p->next = node_p;
    node_p->next = after_node_p;
    after_node_p->next = pixels_itr;



    rle_pairs->length+=2;


}

int main()
{

    // for fast implementation FEEP_TEST_FASTEST.
    char * my_file_name = FEEP_TEST_FASTEST;
    char* my_file = FEEP_TEST_FASTEST PGM_FORMAT;

    Canvas* image = print_PGM(my_file);

    PROCEDING

    // bu işlem vakit alacaktır.
    printf("proceding RLE compression. this take some time...\n"
            "(take time 1-2 min for 512x512 P5 image ,for process in two hash### interval.)...\n"
            "(take time 30-45 sec for 300x246 P2 image ,for process in two hash### interval.)...\n");
    RLE_PAIRS* rle_pairs = make_rle(image);


    printf("huffman min heap created. getting min element...\n");
    heap_t* heap_test = rle_pairs->max_heap;
    heap_node_t* biggest = _heap_get_max(heap_test);
    printf("Minimum number of repeating elements in sequential order:  %ld\n", biggest->k);
    // printf("flow is success\n");

    PROCEDING

    printf("huffman tree creating...\n");


    HuffmanNode* huff_root = create_huffman_tree(rle_pairs->max_heap);

    // see some output for debug purpose

    printf("sample flow...\n");
    printf("%d %c\n", huff_root->frequency, huff_root->state);
    printf("%d %c\n", huff_root->left_node->frequency, huff_root->left_node->state);
    printf("%d %c\n", huff_root->right_node->frequency, huff_root->right_node->state);
    printf("sample flow finished.\n");

    PROCEDING

    printf("huffman table creating...\n");
    // create huffman encoding-decoding table 
    Huffman_Table *ht = make_huffman_table();
    create_huffman_table( ht, huff_root ); // :: optimize hash table TODO

    PROCEDING

    printf("huffman table flushing...\n\n");
    display_huffman_table( ht ); // length show needed
    printf("\nhuffman table flow finished.\n");

    PROCEDING

    // some code here

    /* compressed format filename_cmp.pgm */
    char* compressed_file = (char*) malloc(sizeof(char)* CHUNK);
    strcpy(compressed_file, my_file_name);
    strcat(compressed_file, ADDON_CMP); // compressed addon
    strcat(compressed_file, CMP_FORMAT);

    write_rle_pair2file_h( compressed_file, rle_pairs , ht, image->width, image->height, image->color_bandwidth );

    PROCEDING

    printf("your compressed filename :  %s \n", compressed_file);
    RLE_PAIRS * uncompressed_image = open_compressed_file_with_img_sz( compressed_file, huff_root, image->width, image->height, image->color_bandwidth );


    // checks control
    if( uncompressed_image == NULL ) 
    {
        printf("Error: uncompress failed.\n");
        return -1;
    }

    display_uncompressed_image_unchecked(uncompressed_image);

    PROCEDING

    
    char* u_image_filename = my_file; // this can be modify

    printf("writing a pgm format : %s( please check same file with code.)\n", u_image_filename );

    write_pgm2_file(uncompressed_image, u_image_filename);

    PROCEDING

    printf("do you wanna change color ?(y/n) : ");
    char cntrl;
    scanf("%c", &cntrl);
    if(cntrl == YES_ || cntrl == YES){
        printf("changing color 7 to 15\n");
        int color2chg =15, color = 7;
        change_color( uncompressed_image, color2chg, color );

        PROCEDING

        printf("writing a pgm format : %s( please check same file with code.)\n", u_image_filename );
        write_pgm2_file( uncompressed_image, u_image_filename);
    }

    PROCEDING
    printf("Do you wanna change pixel of given row,column of image? \n");
    
    int row, column, pixel;
    
    printf("give row : ");
    scanf("%d", &row);
    printf("give column : ");
    scanf("%d", &column);
    printf("give pixel: ");
    scanf("%d", &pixel);
    
    change_row_colomn_2pixel( uncompressed_image, row, column, pixel);
    printf("writing a pgm format : %s( please check same file with code.)\n", u_image_filename );
    write_pgm2_file( uncompressed_image, u_image_filename);

    PROCEDING

    printf("Histogram is creating...\n");
    Histogram * u_img_histogram = create_histogram(uncompressed_image);
    display_histogram(u_img_histogram);

    PROCEDING

    printf("ALL PROCESS END.\n");
    


    // for debugging
    // check images 0, 255
    // printf("security scan (y/n) : ");
    // char y_n = getchar();
    
    // PROCEDING
    
    // if ( y_n == YES || y_n == YES_ )
    // {
    //     printf("We scanning your file...\n");

    //     if ( !check_legal_0_255_pix(uncompressed_image) )
    //     {
    //         printf("your image is not approval.(it is not interval of [0-255].)\n");
    //         return -1;
    //     }

    //     // check sequential coincidence
    //     if ( !check_order_rle(uncompressed_image) )
    //     {
    //         printf("your image is not approval.(two same p   if ( !check_legal_0_255_pix(uncompressed_image)" );
    //     }

    // }
    


  return 0;

}
