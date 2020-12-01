
#include <Rcpp.h>
using namespace Rcpp;

#include <QQmlEngine>
#include <QQuickRenderControl>

// [[Rcpp::export]]
List runQml() 
{

	QQuickRenderControl renderControl();

    CharacterVector x = CharacterVector::create( "foo", "bar" )  ;
    NumericVector y   = NumericVector::create( 0.0, 1.0 ) ;
    List z            = List::create( x, y ) ;

    return z ;
}
