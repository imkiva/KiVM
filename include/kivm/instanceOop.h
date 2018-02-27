//
// Created by kiva on 2018/2/28.
//

#include <kivm/oop.h>
#include <kivm/instanceKlass.h>
#include <vector>

namespace kivm {
    class instanceOopDesc : public oopDesc {
        std::vector<oop> _instance_field_values;

    public:
        explicit instanceOopDesc(InstanceKlass *klass);

        ~instanceOopDesc() override = default;
    };
}