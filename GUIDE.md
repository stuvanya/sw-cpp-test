# Руководство разработчика

## 1. Как собрать

Требования: `clang++ 15+`, `cmake 3.27+`, Ubuntu (или macOS).

```bash
cmake -S . -B build
cmake --build build
```

Бинарный файл появится в `build/sw_battle_test`.

---

## 2. Как запустить

```bash
./build/sw_battle_test <путь_к_файлу_сценария>
```

Пример с готовым сценарием:

```bash
./build/sw_battle_test commands_example.txt
```

События выводятся в `stdout` в формате:

```
[<тик>] <ИМЯ_СОБЫТИЯ> поле1=значение1 поле2=значение2 ...
```

---

## 3. Как добавить нового юнита и действие

Всё делается только в `src/Features/` — код в `src/Core/` трогать не нужно.

### Шаг 1. Создать действие в `src/Features/Actions/`

Создайте `MyAction.hpp` и `MyAction.cpp`:

```cpp
// MyAction.hpp
#pragma once
#include <Core/IAction.hpp>
#include <Core/MarchState.hpp>
#include <memory>

namespace sw {
    class MyAction : public IAction {
    public:
        MyAction(uint32_t power, std::shared_ptr<MarchState> marchState);
        void execute(Unit& self, GameContext& ctx) override;
        bool canAct(const Unit& self, const GameContext& ctx) const override;
    private:
        uint32_t                    _power;
        std::shared_ptr<MarchState> _movable;
    };
}
```

```cpp
// MyAction.cpp
#include "MyAction.hpp"
#include "MoveAction.hpp"
#include <Core/GameContext.hpp>
#include <Core/Unit.hpp>

namespace sw {
    MyAction::MyAction(uint32_t power, std::shared_ptr<MarchState> marchState)
        : _power(power), _movable(std::move(marchState)) {}

    bool MyAction::canAct(const Unit& self, const GameContext& ctx) const {
        // вернуть true, если есть цели для атаки или активный марш
        return _movable->hasTarget
            && (self.x != _movable->targetX || self.y != _movable->targetY);
    }

    void MyAction::execute(Unit& self, GameContext& ctx) {
        // логика атаки / лечения / особого поведения
        // ...

        // если некого атаковать — двигаться к цели марша:
        MoveAction(_movable).execute(self, ctx);
    }
}
```

Для атаки используйте `ctx.units.all()` чтобы найти цели,
`ctx.map` чтобы проверить расстояния,
`ctx.eventLog.log(ctx.tick, io::UnitAttacked{...})` чтобы залогировать событие.

### Шаг 2. Создать фабричную функцию в `src/Features/Units/`

Создайте `MyUnit.hpp`:

```cpp
// MyUnit.hpp
#pragma once
#include <Features/Actions/MyAction.hpp>
#include <Core/IAction.hpp>
#include <Core/MarchState.hpp>
#include <Core/Unit.hpp>
#include <memory>

namespace sw {
    inline std::shared_ptr<Unit> makeMyUnit(
        uint32_t id, uint32_t x, uint32_t y, uint32_t hp, uint32_t power)
    {
        auto unit       = std::make_shared<Unit>(id, x, y, static_cast<int32_t>(hp));
        auto marchState = std::make_shared<MarchState>();
        auto action     = std::make_shared<MyAction>(power, marchState);

        unit->components.add<MarchState>(marchState);     // нужен для команды MARCH
        unit->components.add<MyAction, IAction>(action);  // нужен для TurnManager
        return unit;
    }
}
```

> Если юнит **статичный** (не может двигаться) — не создавайте `MarchState`
> и не регистрируйте его. Команда `MARCH` для такого юнита будет молча игнорироваться.

### Флаги поведения юнита

В [`src/Core/Unit.hpp`](src/Core/Unit.hpp) определены два флага, которые можно выставить сразу после создания объекта:

| Флаг | Тип | По умолчанию | Описание |
|------|-----|-------------|----------|
| `occupiesCell` | `bool` | `true` | Если `false` — юнит **не занимает клетку** на карте. Другие юниты могут проходить через неё. Используется для летающих/невидимых юнитов. |
| `targetable` | `bool` | `true` | Если `false` — юнит **не может быть атакован** ни ближним, ни дальним боем. Используется для объектов, которые нельзя уничтожить напрямую. |

Пример использования в фабричной функции:

```cpp
auto unit = std::make_shared<Unit>(id, x, y, static_cast<int32_t>(hp));
unit->occupiesCell = false;  // Ворон: не блокирует клетки
unit->targetable   = false;  // Мина: нельзя атаковать напрямую
```

> **Важно**: если `occupiesCell = false`, не вызывайте `map.occupy()` при спауне
> и не вызывайте `map.free()`/`map.occupy()` при движении — это уже обрабатывается
> автоматически в [`MoveAction.cpp`](src/Features/Actions/MoveAction.cpp) и
> [`main.cpp`](src/main.cpp).

### Шаг 3. Добавить команду спауна в `src/IO/Commands/`

По образцу [`SpawnSwordsman.hpp`](src/IO/Commands/SpawnSwordsman.hpp) создайте структуру:

```cpp
// SpawnMyUnit.hpp
#pragma once
#include <cstdint>

namespace sw::io {
    struct SpawnMyUnit {
        constexpr static const char* Name = "SPAWN_MY_UNIT";

        uint32_t unitId{};
        uint32_t x{};
        uint32_t y{};
        uint32_t hp{};
        uint32_t power{};

        template <typename Visitor>
        void visit(Visitor& visitor) {
            visitor.visit("unitId", unitId);
            visitor.visit("x", x);
            visitor.visit("y", y);
            visitor.visit("hp", hp);
            visitor.visit("power", power);
        }
    };
}
```

### Шаг 4. Зарегистрировать команду в `src/main.cpp`

```cpp
#include <Features/Units/MyUnit.hpp>
#include <IO/Commands/SpawnMyUnit.hpp>
// ...
parser.add<io::SpawnMyUnit>([&](auto cmd) {
    if (!map.isInBounds(cmd.x, cmd.y))
        throw std::runtime_error("SPAWN_MY_UNIT: coordinates out of map bounds");
    if (map.isOccupied(cmd.x, cmd.y))
        throw std::runtime_error("SPAWN_MY_UNIT: cell is already occupied");

    auto unit = makeMyUnit(cmd.unitId, cmd.x, cmd.y, cmd.hp, cmd.power);
    units.add(unit);
    if (unit->occupiesCell)
        map.occupy(cmd.x, cmd.y);
    eventLog.log(ctx.tick, io::UnitSpawned{cmd.unitId, "MyUnit", cmd.x, cmd.y});
});
```

После этого пересоберите проект (`cmake --build build`) — новый юнит готов к использованию в сценариях.

---

## 4. Тесты

### Структура

```
tests/
├── TestRunner.hpp       # минималистичный тест-раннер (без внешних зависимостей)
├── TestHelpers.hpp      # GameFixture — готовый GameContext с тихим EventLog
├── main_test.cpp        # точка входа тестового бинарника
├── test_Map.cpp         # юнит-тесты Map
├── test_UnitStorage.cpp # юнит-тесты UnitStorage
├── test_MoveAction.cpp  # юнит-тесты MoveAction
├── test_MeleeAttack.cpp # юнит-тесты MeleeAttackAction
├── test_RangedAttack.cpp# юнит-тесты RangedAttackAction
├── test_TurnManager.cpp # юнит-тесты TurnManager
└── test_Integration.cpp # интеграционные сценарии
```

### Сборка и запуск

```bash
# Сконфигурировать (один раз)
cmake -S . -B build_tests

# Собрать тестовый бинарник
cmake --build build_tests --target sw_tests

# Запустить тесты
./build_tests/sw_tests
```

Ожидаемый вывод:

```
  OK    Integration_swordsman_kills_adjacent_swordsman_in_one_hit
  OK    Map_isInBounds_true_for_valid_coordinates
  ...
48 passed, 0 failed (94 checks total)
```

Для запуска через CTest:

```bash
cd build_tests && ctest --output-on-failure
```

### Как писать новые тесты

Синтаксис тестов аналогичен Google Test — имя теста является C++ идентификатором,
`END_TEST` не нужен. Используйте `GameFixture` из [`tests/TestHelpers.hpp`](tests/TestHelpers.hpp) —
он создаёт `Map`, `UnitStorage` и `EventLog` в тихом режиме (без вывода в stdout).
RNG сбрасывается в seed 0 вызовом `ctx.rng.seed(0)` в конструкторе фикстуры,
что делает результаты детерминированными.

```cpp
#include "TestRunner.hpp"
#include "TestHelpers.hpp"
#include <Features/Units/SwordsmanUnit.hpp>

using namespace sw;
using namespace sw::test;

TEST(MyFeature_describe_what_is_tested)
{
    GameFixture f;                               // карта 10×10, тихий лог, seed=0

    auto unit = makeSwordsman(1, 0, 0, 100, 10);
    f.units.add(unit);
    f.map.occupy(0, 0);

    // ... действия ...

    CHECK(f.eventLog.hasEvent("UNIT_ATTACKED")); // проверить событие
    CHECK_EQ(unit->hp, int32_t(90));             // проверить значение
}
```

Доступные хелперы `EventLog`:

| Метод | Описание |
|-------|----------|
| `hasEvent("NAME")` | Было ли залогировано хотя бы одно событие с таким именем |
| `countEvents("NAME")` | Сколько раз событие было залогировано |
| `lastEventName()` | Имя последнего залогированного события |
| `captured()` | Весь вектор `Entry{tick, name}` для детальной проверки |
