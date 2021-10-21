 
#include <kcmodule.h>

class ShapeCornersConfig : public KCModule
{
    Q_OBJECT
public:
    explicit ShapeCornersConfig(QWidget* parent = 0, const QVariantList& args = QVariantList());
    ~ShapeCornersConfig();
public slots:
    void save();
    void load();
    void defaults();

private:
    class Private;
    Private * const d;
    friend class Private;
};
