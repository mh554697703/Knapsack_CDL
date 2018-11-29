#include "stgraph.h"

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap( Qt::white, Qt::darkRed )
    {
        addColorStop( 0,    Qt::white);
        addColorStop( 0.01, Qt::blue );
        addColorStop( 0.4,  Qt::cyan );
        addColorStop( 0.6,  Qt::yellow );
        addColorStop( 0.8,  Qt::red );
    }
};

STGraph::STGraph( QWidget *parent ):
    QwtPlot( parent )
{
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );                         //画布四周边框角半径
    setCanvas( canvas );

    d_spectrogram = new QwtPlotSpectrogram();

    setAxisScale( QwtPlot::xBottom, -10, 0 );
    setAxisMaxMinor( QwtPlot::xBottom, 0 );               //每个大格子中有0个小格子
    //    ColorMap *myColorMap = new ColorMap();
    d_spectrogram->setColorMap( new ColorMap());

    QFont font("Microsoft Yahei",12);
    font.setBold(true);
    setAxisFont(QwtPlot::yLeft,font);
    setAxisFont(QwtPlot::xBottom,font);

    //    QList<double> ticks;
    //    ticks << 132 << 220 << 308 << 396 << 484 << 572 << 660 << 748 << 836 << 924 << 1012 << 1100 << 1188 << 1276;
    ////    ticks << 132 << 220 << 308 << 396 << 484 << 572 << 660 << 748 << 836 << 924 << 1012 << 1100;
    //    QwtScaleDiv scaleDiv(88, 1320);
    //    scaleDiv.setTicks(QwtScaleDiv::MajorTick, ticks);
    //    setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);

    stData = new STData;
}

void STGraph::initialShow(uint nLayers, double minHeight, double heightStep)
{
    stData->setSize(nLayers);
    stData->setdata(new double[nLayers]{0});
    stData->setHeightsLabel(minHeight, minHeight + (nLayers-1)*heightStep);

    QList<double> ticks;
    //    ticks << 132 << 220 << 308 << 396 << 484 << 572 << 660 << 748 << 836 << 924 << 1012 << 1100 << 1188 << 1276;
    //    ticks << 132 << 220 << 308 << 396 << 484 << 572 << 660 << 748 << 836 << 924 << 1012 << 1100;
    for(uint i=0;i<nLayers;i++) {
        ticks += minHeight + i*heightStep;
    }
    QwtScaleDiv scaleDiv(minHeight-heightStep/2, minHeight + (nLayers-1)*heightStep+heightStep/2);
    scaleDiv.setTicks(QwtScaleDiv::MajorTick, ticks);
    setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);

    d_spectrogram->setData( stData );
    d_spectrogram->attach(this);
//    setAxisScale( QwtPlot::yLeft, minHeight, minHeight + (nLayers-1)*heightStep, heightStep );

    //    static_cast<STData *>( d_spectrogram->data() )->setResampleMode( static_cast<QwtMatrixRasterData::ResampleMode>( 0 ));
    stData->setResampleMode(QwtMatrixRasterData::NearestNeighbour);

    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, false);       //设置显示模式
    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, true);

    //z轴
    zInterval.setInterval(0,15);
    // A color bar on the right axis
    //    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorBarWidth( 20 );
    rightAxis->setColorMap( zInterval, new ColorMap());
    QFont font("Microsoft Yahei",12);
    font.setBold(true);
    rightAxis->setFont(font);
    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    enableAxis( QwtPlot::yRight );
}

void STGraph::updateShow(const double *data_in)
{
    stData->setdata(data_in);
    d_spectrogram->setData( stData );
    d_spectrogram->attach(this);

    zInterval = d_spectrogram->data()->interval( Qt::ZAxis );
    rightAxis->setColorMap( zInterval, new ColorMap() );
    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );

    replot();

}

