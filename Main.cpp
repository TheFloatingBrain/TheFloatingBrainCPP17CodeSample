#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <vector>
#include <optional>
#include <array>
 
namespace Detail
{
    template< size_t COLUMBS_PARAMETER_CONSTANT = 2,
            size_t ROWS_PARAMETER_CONSTANT = 2, typename SCALER_TYPE = int >
    struct Matrix
    {
        const size_t COLUMBS_CONSTANT = COLUMBS_PARAMETER_CONSTANT;
        const size_t ROWS_CONSTANT = ROWS_PARAMETER_CONSTANT;
        SCALER_TYPE** innerMatrix;
        Matrix( std::optional< std::array< std::array< SCALER_TYPE, COLUMBS_PARAMETER_CONSTANT >,
                ROWS_PARAMETER_CONSTANT > > initialValues )
        {
            innerMatrix = new SCALER_TYPE*[ COLUMBS_PARAMETER_CONSTANT ];
            for( size_t i = 0; i < COLUMBS_CONSTANT; ++i )
            {
                innerMatrix[ i ] = new SCALER_TYPE[ ROWS_CONSTANT ];
                if( initialValues ) {
                    for( int j = 0; j < ROWS_PARAMETER_CONSTANT; ++j )
                        innerMatrix[ i ][ j ] = initialValues.value()[ i ][ j ];
                }
            }
        }
        ~Matrix()
        {
            for( size_t i = 0; i < COLUMBS_CONSTANT; ++i )
                delete innerMatrix[ i ];
            delete innerMatrix;
        }
        SCALER_TYPE* operator[] ( size_t index ) {
            return innerMatrix[ index ];
        }
    };
}
template< size_t COLUMBS_CONSTANT = 2,
            size_t ROWS_CONSTANT = 2, typename SCALER_TYPE = int >
using MATRIX_TYPE = typename std::unique_ptr< Detail::Matrix< COLUMBS_CONSTANT,
        ROWS_CONSTANT, SCALER_TYPE > >;
 
template< size_t COLUMBS_CONSTANT = 2,
            size_t ROWS_CONSTANT = 2, typename SCALER_TYPE = int >
MATRIX_TYPE< COLUMBS_CONSTANT, ROWS_CONSTANT, SCALER_TYPE >
        MakeMatrix( std::optional< std::array< std::array< SCALER_TYPE, COLUMBS_CONSTANT >,
                ROWS_CONSTANT > > initialValues ) {
    return std::make_unique< Detail::Matrix< COLUMBS_CONSTANT,
            ROWS_CONSTANT, SCALER_TYPE > >( initialValues );
}
 
//Shorthand for the purposes of this example.//
template< size_t COLUMBS_CONSTANT = 2, size_t ROWS_CONSTANT = 2 >
using Matrix = MATRIX_TYPE< COLUMBS_CONSTANT, ROWS_CONSTANT, int >;
template< size_t COLUMBS_CONSTANT = 2, size_t ROWS_CONSTANT = 2 >
using Islands = MATRIX_TYPE< COLUMBS_CONSTANT, ROWS_CONSTANT, bool >;
 
template< typename DISPLAY_MATRIX_TYPE >
void DisplayMatrix( DISPLAY_MATRIX_TYPE matrix )
{
    std::cout << "[ ";
    for( size_t i = 0; i < 5; ++i )
    {
        for( size_t j = 0; j < 5;
            std::cout << ( *matrix )[ i ][ j++ ] << ( ( i >= 5 && j == 5 )
                    ? "" : ( j >= 5 ? "; " : ", " ) ) );
    }
    std::cout << "]\n";
}
 
 
 
template< size_t COLUMBS_CONSTANT = 2, size_t ROWS_CONSTANT = 2 >
Islands< COLUMBS_CONSTANT, ROWS_CONSTANT >
        FindIslands( const Matrix< COLUMBS_CONSTANT, ROWS_CONSTANT >& from,
                int threshold, size_t minimumSize )
{
    auto islandMatrix = MakeMatrix< COLUMBS_CONSTANT, ROWS_CONSTANT, bool >( {} );
    auto countMatrix = MakeMatrix< COLUMBS_CONSTANT, ROWS_CONSTANT >( {} );
    for( size_t i = 0; i < ROWS_CONSTANT; ++i )
    {
        size_t lastContinous = 0;
        /*********************************************************************
        ** NOTE: According to the C++ standard, all int to bool conversions **
        ** are either 1 or 0.  This block first figures out if the current ***
        ** number is above the threshold. If it is then it adds 1 to *********
        ** lastContinous and assignes lastContinous to that value as well ****
        ** as countMatrix[ i ][ j ]. If not then it assignes them both to ****
        ** zero, after that it addresses the current row if i == 0 and *******
        ** the previous row otherwise (i.e ( ( int ) ( ( bool ) i ) ), but ***
        ** otherwise addresses the current row if the current number is ******
        ** less than or equal to the threshold e.i multiplying by ************
        ** ( ( int ) ( ( bool ) ( *countMatrix )[ i ][ j ] ) ) . *************
        ** this works because ( *countMatrix )[ i ][ j ] ) has already been **
        ** assigned, and sequence points was eliminated in C++11. ************
        ** Once the correct row has been addressed, the current columb is ****
        ** j, whatever is in this location (one up if there is a previous ****
        ** row and the current number is greater than or equal to the ********
        ** threshold). If the current row is added it means the element ******
        ** contained there in is 0, so there is no vertical island! At the ***
        ** end, j is incremented and the current element ( i, j ) is *********
        ** assigned. For the record, I would never do this in production *****
        ** code... unless absolutly nessisary. *******************************
        *********************************************************************/
        for( size_t j = 0; j < COLUMBS_CONSTANT;
                    ( *countMatrix )[ i ][ j++ ] = ( ( lastContinous = (
                    ( *countMatrix )[ i ][ j ] = ( ( *from )[ i ][ j ]
                    >= threshold ) ) ? ( lastContinous + 1 ) : 0 ) +
                    ( *countMatrix )[ ( i -
                    ( ( int ) ( ( bool ) i ) ) * ( ( int ) ( ( bool ) (
                    *countMatrix )[ i ][ j ] ) ) ) ][ j ] ) )
            ( *islandMatrix )[ i ][ j ] = false;
        //j goes below 0, so using the int type.//
        for( int j = ( COLUMBS_CONSTANT - 1 ); j >= 0; --j )
        {
            if( ( *countMatrix )[ i ][ j ] != 0 )
            {
                if( ( *countMatrix )[ i ][ j ] >= minimumSize )
                {
                    ( *islandMatrix )[ i ][ j ] = true;
                    if( j > 0 )
                    {
                        if( ( *countMatrix )[ i ][ j - 1 ] != 0 &&
                                ( *countMatrix )[ i ][ j - 1 ] < minimumSize )
                        {
                            /******************************************************************************
                            ** Shouldent be n^3 only does for short subsections and does not repeat work **
                            ** only needs to compute an operation once. ***********************************
                            ******************************************************************************/
                            for( int o = ( j - 1 ); o >= 0; --o )
                            {
                                /***********************************************
                                ** Can stop if the minimumSize condition is ****
                                ** met because: ********************************
                                **   0: It would have already become true ******
                                **   1: This loop would have been run on any ***
                                **       any preciding values below the ********
                                **       minimumSize. **************************
                                ***********************************************/
                                if( ( *countMatrix )[ i ][ o ] == 0 ||
                                        ( *countMatrix )[ i ][ o ] >= minimumSize )
                                    break;
                                ( *islandMatrix )[ i ][ o ] = true;
                            }
                        }
                    }
                    if( i >= ( minimumSize - 1 ) )
                    {
                        if( ( *countMatrix )[ i - 1 ][ j ] < minimumSize &&
                                ( *countMatrix )[ i - 1 ][ j ] != 0 )
                        {
                            for( int o = ( i - 1 ); o >= 0; --o )
                            {
                                if( ( *countMatrix )[ o ][ j ] >= minimumSize ||
                                        ( *countMatrix )[ o ][ j ] == 0 )
                                    break;
                                ( *islandMatrix )[ o ][ j ] = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return islandMatrix;
}
 
std::array< std::array< int, 5 >, 5 > testCase{ {
                { 4, 4, 4, 2, 2 },
                { 4, 2, 2, 2, 2 },
                { 2, 2, 8, 7, 2 },
                { 2, 8, 8, 8, 2 },
                { 8, 2, 2, 2, 8 } } };
 
int main()
{
    auto input = MakeMatrix< 5, 5 >( std::optional< decltype( testCase ) >{ testCase } );
    DisplayMatrix< decltype( input )& >( input );
    auto result =
            FindIslands< 5, 5 >( input, 5, 3 );
    DisplayMatrix< decltype( result )& >( result );
    return 0;
}
