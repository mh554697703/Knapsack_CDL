#ifndef STGRAPH_H
#define STGRAPH_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <QVector>
#include <qwt_matrix_raster_data.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_layout.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_zoomer.h>
#include <qtimer.h>
#include <qwt_date_scale_draw.h>
#include "stdata.h"

class STGraph: public QwtPlot
{
    Q_OBJECT

public:
    STGraph( QWidget * = nullptr );

    void initialShow(uint layernum, double minHeight, double heightStep);
    void updateShow(const double *s);

private:
    QwtPlotSpectrogram *d_spectrogram;         //QwtPlotSpectrogram ÆµÆ×Í¼Àà
    STData *stData;
    QwtScaleWidget *rightAxis;
    QwtInterval zInterval;

};

#endif // STGRAPH_H
