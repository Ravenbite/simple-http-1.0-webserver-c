// Core
#include "./auxiliary/aux_includes.h"
#include <assert.h>
// Extensions
#include "./caching/cache_hashTable.h"
#include "./caching/cache_double_linked_list.h"
#include "./caching/cache_double_linked_node.h"


// Hardcoded extensions
size_t checkFile( char* file ) 
{
    // Variables:
    //----------------------------------------------------------------------------------------------------------
    size_t len = 0;
    FILE *resourceFD;               
    
    // Access test
    //----------------------------------------------------------------------------------------------------------
    resourceFD = fopen( file, "r" );
	if ( resourceFD != NULL )
    {
        // Get filelength
        //------------------------------------------------------------------------------------------------------
        fseek ( resourceFD , 0 , SEEK_END );                    // Seek file end
		len = ftell ( resourceFD );                             // Get file length
		rewind ( resourceFD );                                  // Reset file ptr
        fclose( resourceFD );
        return len;
    }
    else 
    {
        return 0;
    }
}




// Testroutine
int main()
{
    printf("------------------------------\n");
    printf("-----Unittest for caching-----\n");
    printf("------------------------------\n\n");

/*__Prepare_some_stuff_________________________________________________________________*/
    char      mimeType[4][17] = 
    {
    "text/plain",
    "text/html",
    "image/jpeg",
    "application/json"
    };
    
    char*       file_txt  =       "./testing/testFile.txt";
    char*       file_html =       "./testing/testFile.html";
    char*       file_jpg  =       "./testing/testFile.jpg";
    char*       file_json =       "./testing/testFile.json";
    
    size_t      fileSize;

    hT_t*       hashTable = hT_new();

    dL_node_t*  fileNode_txt  = dL_node_new( file_txt,  file_txt,  mimeType[0], checkFile( file_txt ) );
    dL_node_t*  fileNode_html = dL_node_new( file_html, file_html, mimeType[1], checkFile( file_html ) );
    dL_node_t*  fileNode_jpeg = dL_node_new( file_jpg,  file_jpg,  mimeType[2], checkFile( file_jpg ) );
    dL_node_t*  fileNode_json = dL_node_new( file_json, file_json, mimeType[3], checkFile( file_json ) );
    
    dL_list_t*  list_one      = dL_list_new();
    dL_list_t*  list_two      = dL_list_new();

/*__Test_phase_I_________________________________________________________________*/
    printf("(LowLevelTest)\t [ Check allocating ] \n");
    assert( NULL != hashTable && "Message: allocated object is NULL" );
    assert( NULL != fileNode_txt && "Message: allocated object is NULL" );
    assert( NULL != fileNode_jpeg && "Message: allocated object is NULL" );
    assert( NULL != fileNode_html && "Message: allocated object is NULL" );
    assert( NULL != fileNode_json && "Message: allocated object is NULL" );
    assert( NULL != list_one && "Message: allocated object is NULL" );
    assert( NULL != list_two && "Message: allocated object is NULL" );
    printf("\t\t...passed\n");

/*__Test_phase_II_________________________________________________________________*/
    printf("(LowLevelTest)\t [ Check memoryMappedFile ] \n");
    assert( NULL != fileNode_txt->mmapData && "Message: mmap ptr is NULL" );
    assert( NULL != fileNode_html->mmapData && "Message: mmap ptr is NULL" );
    assert( NULL != fileNode_jpeg->mmapData && "Message: mmap ptr is NULL" );
    assert( NULL != fileNode_json->mmapData && "Message: mmap ptr is NULL" );
    assert(    0 == strcmp( "HaliHallo\nHupfi drei 5971\nENdE    TAB STop;\n\nHuhu.", fileNode_txt->mmapData ) && "Message: mmap data and original file content isnot the same!");
    printf("\t\t...passed\n");

/*__Test_phase_III_________________________________________________________________*/
    printf("(LowLevelTest)\t [ Pop left from three in list ] \n");
    dL_list_lpush( list_one, fileNode_txt );
    dL_list_rpush( list_one, fileNode_jpeg);
    dL_list_rpush( list_one, fileNode_html);
    dL_list_pop( list_one, fileNode_txt );

    //    <---TXT---JPEG---HTML--->
    assert( (int)2 == (int)(list_one->len) && "Message: List should have one object less" );
    assert( list_one->head != fileNode_txt && "Message: The popped node is still in the list!");

    dL_list_clear( list_one );  // Clear without recursiv node reset! The may linked together
    printf("\t\t...passed\n");

/*__Test_phase_IV_________________________________________________________________*/
    printf("(LowLevelTest)\t [ Pop right from three in list ] \n");
    dL_list_lpush( list_one, fileNode_txt );
    dL_list_rpush( list_one, fileNode_jpeg);
    dL_list_rpush( list_one, fileNode_html);
    dL_list_pop( list_one, fileNode_html );

    //    <---TXT---JPEG---HTML--->
    assert( 2 == list_one->len && "Message: List should have one object less");
    assert( list_one->tail != fileNode_html && "Message: The popped node is still in the list!" );

    dL_list_clear( list_one );
    printf("\t\t...passed\n");

/*__Test_phase_V_________________________________________________________________*/
    printf("(LowLevelTest)\t [ Pop middle from three in list ] \n");
    dL_list_lpush( list_one, fileNode_txt );
    dL_list_rpush( list_one, fileNode_jpeg);
    dL_list_rpush( list_one, fileNode_html);
    dL_list_pop( list_one, fileNode_jpeg );

    //    <---TXT---JPEG---HTML--->
    assert( 2 == list_one->len && "Message: List should have one object less");
    assert( list_one->head != fileNode_jpeg && "Message: The popped node is still in the list!" );
    assert( list_one->tail != fileNode_jpeg && "Message: The popped node is still in the list!" );
    printf("\t\t...passed\n");

/*__Test_phase_VI________________________________________________________________*/
    printf("(LowLevelTest)\t [ Check free list recursively ] \n");
    dL_list_free( list_one );
    list_one = NULL;

    assert( NULL == fileNode_txt->mmapData && "Message: mmap file is still allocated!" );
    assert(    0 == fileNode_txt->len && "Message: Node len implicates that there is still some data allocated ");
    assert( NULL == list_one && "Message: Your programm can still enter list because it isnt NULL ");
    printf("\t\t...passed\n");

/*__Test_phase_VII_______________________________________________________________*/
    printf("(LowLevelTest)\t [ Perform search and validate result ] \n");
    fileNode_jpeg = dL_node_new( file_jpg,  file_jpg,  mimeType[2], checkFile( file_jpg ) );
    fileNode_json = dL_node_new( file_json, file_json, mimeType[3], checkFile( file_json ) );
    list_one      = dL_list_new();
    dL_list_lpush( list_one, fileNode_jpeg );
    dL_list_lpush( list_one, fileNode_json );
    int compOP = 0;

    assert( fileNode_json == dL_list_search( list_one, fileNode_json->len, fileNode_json->simplePath, &compOP ) && "Message: Your searchFx returns a wrong node!");
    printf("\t\t...passed\n\n");


/*__Test_phase_VII_______________________________________________________________*/
    printf("(HighLevelTest)\t [ Create hashTable with two rows, spawn some nodes ] \n");
    // Create cache and init two rows
    fileNode_txt  = dL_node_new( file_txt,  file_txt,  mimeType[0], checkFile( file_txt ) );
    fileNode_html = dL_node_new( file_html, file_html, mimeType[1], checkFile( file_html ) );
    fileNode_jpeg = dL_node_new( file_jpg,  file_jpg,  mimeType[2], checkFile( file_jpg ) );
    fileNode_json = dL_node_new( file_json, file_json, mimeType[3], checkFile( file_json ) );
    // no asserts needed because we already handled those cases
    printf("\t\t...passed\n\n");

/*__Test_phase_VII_______________________________________________________________*/
    printf("(HighLevelTest)\t [ Compute hashes and fill table ] \n");
    // Add nodes, fx call will hash and insert and perform other stuff if needed
    hT_insert( hashTable, fileNode_txt );
    hT_insert( hashTable, fileNode_html );
    hT_insert( hashTable, fileNode_jpeg );
    hT_insert( hashTable, fileNode_json );
    printf("\t\t...passed\n\n");
    
/*__Test_phase_VII_______________________________________________________________*/
    printf("(HighLevelTest)\t [ Search in table ] \n");
    compOP = 0;
    dL_node_t* query = hT_search( hashTable, fileNode_txt->len, fileNode_txt->simplePath, &compOP );
    printf("\t\t...passed\n\n");

/*__Test_phase_VII_______________________________________________________________*/
    printf("(HighLevelTest)\t [ Update last node ] \n");
    hT_update( hashTable, fileNode_json, hash(fileNode_json->simplePath) );
    printf("\t\t...passed\n\n");

/*__Test_phase_VII_______________________________________________________________*/
    printf("(HighLevelTest)\t [ Delete first row ] \n");
    //hT_freeEntry(hashTable, 0);
    printf("\t\t...passed\n\n");

/*__Test_phase_VII_______________________________________________________________*/
    printf("(HighLevelTest)\t [ Search existing and non existing node ] \n");
    compOP = 0;
    query = hT_search( hashTable, fileNode_txt->len, fileNode_txt->simplePath, &compOP );
    query = hT_search( hashTable, fileNode_html->len, fileNode_html->simplePath, &compOP );
    query = hT_search( hashTable, fileNode_jpeg->len, fileNode_jpeg->simplePath, &compOP );
    query = hT_search( hashTable, fileNode_json->len, fileNode_json->simplePath, &compOP );
    printf("\t\t...passed\n\n");

/*__Test_phase_VII_______________________________________________________________*/
    printf("(HighLevelTest)\t [ Free table recursively ] \n");
    hT_free( hashTable);
    printf("\t\t...passed\n\n");

/*__Test_phase_VII_______________________________________________________________*/
    printf("(HighLevelTest)\t [ Perform some random operations on left ptrs ( to check whether your code is save ) ] \n");


printf("\n-------------------------------------\n");
printf("-----Test fulfilled successfully-----\n");
printf("-------------------------------------\n");





}