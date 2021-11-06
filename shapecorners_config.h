 
#include <kcmodule.h>

class ShapeCornersConfig : public KCModule
{
    Q_OBJECT
public:
    explicit ShapeCornersConfig(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
    ~ShapeCornersConfig() override;
public slots:
    void save() override;
    void load() override;
    void defaults() override;

private:
    class Private;
    Private * const d;
    friend class Private;
};
