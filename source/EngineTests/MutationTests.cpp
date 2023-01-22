#include <cmath>

#include <algorithm>
#include <ranges>
#include <boost/range/combine.hpp>

#include <gtest/gtest.h>

#include "EngineInterface/DescriptionHelper.h"
#include "EngineInterface/Descriptions.h"
#include "EngineInterface/SimulationController.h"
#include "EngineInterface/GenomeDescriptionConverter.h"

#include "IntegrationTestFramework.h"

class MutationTests : public IntegrationTestFramework
{
public:
    MutationTests()
        : IntegrationTestFramework()
    {}

    ~MutationTests() = default;

protected:
    std::vector<int> const GenomeCellColors = {1, 4, 5};
    std::vector<uint8_t> createGenomeWithMultipleCellsWithDifferentFunctions() const
    {
        std::vector<uint8_t> subGenome;
        for (int i = 0; i < 15; ++i) {
            subGenome = GenomeDescriptionConverter::convertDescriptionToBytes(GenomeDescription{
                CellGenomeDescription().setCellFunction(NeuronGenomeDescription()).setColor(GenomeCellColors[0]),
                CellGenomeDescription().setCellFunction(TransmitterGenomeDescription()).setColor(GenomeCellColors[1]),
                CellGenomeDescription().setColor(GenomeCellColors[2]),
                CellGenomeDescription().setCellFunction(ConstructorGenomeDescription().setMakeGenomeCopy()).setColor(GenomeCellColors[2]),
                CellGenomeDescription().setCellFunction(ConstructorGenomeDescription().setGenome(subGenome)).setColor(GenomeCellColors[0]),
            });
        };
        return GenomeDescriptionConverter::convertDescriptionToBytes({
            CellGenomeDescription().setCellFunction(NeuronGenomeDescription()).setColor(GenomeCellColors[0]),
            CellGenomeDescription().setCellFunction(TransmitterGenomeDescription()).setColor(GenomeCellColors[1]),
            CellGenomeDescription().setColor(GenomeCellColors[0]),
            CellGenomeDescription().setCellFunction(ConstructorGenomeDescription().setMakeGenomeCopy()).setColor(GenomeCellColors[1]),
            CellGenomeDescription().setCellFunction(ConstructorGenomeDescription().setGenome(subGenome)).setColor(GenomeCellColors[0]),
            CellGenomeDescription().setCellFunction(SensorGenomeDescription()).setColor(GenomeCellColors[2]),
            CellGenomeDescription().setCellFunction(NerveGenomeDescription()).setColor(GenomeCellColors[1]),
            CellGenomeDescription().setCellFunction(AttackerGenomeDescription()).setColor(GenomeCellColors[0]),
            CellGenomeDescription().setCellFunction(InjectorGenomeDescription().setGenome(subGenome)).setColor(GenomeCellColors[0]),
            CellGenomeDescription().setCellFunction(MuscleGenomeDescription()).setColor(GenomeCellColors[2]),
            CellGenomeDescription().setCellFunction(PlaceHolderGenomeDescription1()).setColor(GenomeCellColors[2]),
            CellGenomeDescription().setCellFunction(PlaceHolderGenomeDescription2()).setColor(GenomeCellColors[0]),
        });
    }

    bool compareDataMutation(std::vector<uint8_t> const& expected, std::vector<uint8_t> const& actual)
    {
        if (expected.size() != actual.size()) {
            return false;
        }
        auto expectedGenome = GenomeDescriptionConverter::convertBytesToDescription(expected, _parameters);
        auto actualGenome = GenomeDescriptionConverter::convertBytesToDescription(actual, _parameters);
        if (expectedGenome.size() != actualGenome.size()) {
            return false;
        }

        for (auto const& [expectedCell, actualCell] : boost::combine(expectedGenome, actualGenome)) {
            if (expectedCell.getCellFunctionType() != actualCell.getCellFunctionType()) {
                return false;
            }
            if (expectedCell.color != actualCell.color) {
                return false;
            }
            if (expectedCell.getCellFunctionType() == CellFunction_Constructor) {
                auto expectedConstructor = std::get<ConstructorGenomeDescription>(*expectedCell.cellFunction);
                auto actualConstructor = std::get<ConstructorGenomeDescription>(*actualCell.cellFunction);
                if (expectedConstructor.isMakeGenomeCopy() != actualConstructor.isMakeGenomeCopy()) {
                    return false;
                }
                if (!expectedConstructor.isMakeGenomeCopy()) {
                    if (!compareDataMutation(expectedConstructor.getGenomeData(), actualConstructor.getGenomeData())) {
                        return false;
                    }
                }
            }
            if (expectedCell.getCellFunctionType() == CellFunction_Injector) {
                auto expectedInjector = std::get<InjectorGenomeDescription>(*expectedCell.cellFunction);
                auto actualInjector = std::get<InjectorGenomeDescription>(*actualCell.cellFunction);
                if (expectedInjector.isMakeGenomeCopy() != actualInjector.isMakeGenomeCopy()) {
                    return false;
                }
                if (!expectedInjector.isMakeGenomeCopy()) {
                    if (!compareDataMutation(expectedInjector.getGenomeData(), actualInjector.getGenomeData())) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool compareNeuronDataMutation(std::vector<uint8_t> const& expected, std::vector<uint8_t> const& actual)
    {
        if (expected.size() != actual.size()) {
            return false;
        }
        auto expectedGenome = GenomeDescriptionConverter::convertBytesToDescription(expected, _parameters);
        auto actualGenome = GenomeDescriptionConverter::convertBytesToDescription(actual, _parameters);
        if (expectedGenome.size() != actualGenome.size()) {
            return false;
        }

        for (auto const& [expectedCell, actualCell] : boost::combine(expectedGenome, actualGenome)) {
            if (expectedCell.getCellFunctionType() != actualCell.getCellFunctionType()) {
                return false;
            }
            if (expectedCell.getCellFunctionType() != CellFunction_Neuron && expectedCell.getCellFunctionType() != CellFunction_Constructor
                && expectedCell.getCellFunctionType() != CellFunction_Injector && expectedCell != actualCell) {
                return false;
            }
            if (expectedCell.color != actualCell.color) {
                return false;
            }
            if (expectedCell.getCellFunctionType() == CellFunction_Constructor) {
                auto expectedConstructor = std::get<ConstructorGenomeDescription>(*expectedCell.cellFunction);
                auto actualConstructor = std::get<ConstructorGenomeDescription>(*actualCell.cellFunction);
                if (expectedConstructor.isMakeGenomeCopy() != actualConstructor.isMakeGenomeCopy()) {
                    return false;
                }
                if (!expectedConstructor.isMakeGenomeCopy()) {
                    if (!compareNeuronDataMutation(expectedConstructor.getGenomeData(), actualConstructor.getGenomeData())) {
                        return false;
                    }
                }
            }
            if (expectedCell.getCellFunctionType() == CellFunction_Injector) {
                auto expectedInjector = std::get<InjectorGenomeDescription>(*expectedCell.cellFunction);
                auto actualInjector = std::get<InjectorGenomeDescription>(*actualCell.cellFunction);
                if (expectedInjector.isMakeGenomeCopy() != actualInjector.isMakeGenomeCopy()) {
                    return false;
                }
                if (!expectedInjector.isMakeGenomeCopy()) {
                    if (!compareNeuronDataMutation(expectedInjector.getGenomeData(), actualInjector.getGenomeData())) {
                        return false;
                    }
                }
            }

        }
        return true;
    }

    bool compareCellFunctionMutation(std::vector<uint8_t> const& expected, std::vector<uint8_t> const& actual)
    {
        auto expectedGenome = GenomeDescriptionConverter::convertBytesToDescription(expected, _parameters);
        auto actualGenome = GenomeDescriptionConverter::convertBytesToDescription(actual, _parameters);
        if (expectedGenome.size() != actualGenome.size()) {
            return false;
        }
        for (auto const& [expectedCell, actualCell] : boost::combine(expectedGenome, actualGenome)) {
            if (std::abs(expectedCell.referenceDistance - actualCell.referenceDistance) > NEAR_ZERO) {
                return false;
            }
            if (std::abs(expectedCell.referenceAngle - actualCell.referenceAngle) > NEAR_ZERO) {
                return false;
            }
            if (expectedCell.color != actualCell.color) {
                return false;
            }
            if (expectedCell.maxConnections != actualCell.maxConnections) {
                return false;
            }
            if (expectedCell.executionOrderNumber != actualCell.executionOrderNumber) {
                return false;
            }
            if (expectedCell.inputBlocked != actualCell.inputBlocked) {
                return false;
            }
            if (expectedCell.outputBlocked != actualCell.outputBlocked) {
                return false;
            }
        }
        return true;
    }

    bool compareInsertMutation(std::vector<uint8_t> const& before, std::vector<uint8_t> const& after)
    {
        auto beforeGenome = GenomeDescriptionConverter::convertBytesToDescription(before, _parameters);
        auto afterGenome = GenomeDescriptionConverter::convertBytesToDescription(after, _parameters);
        for (auto const& cell : afterGenome) {
            if (std::ranges::find(GenomeCellColors, cell.color) == GenomeCellColors.end()) {
                return false;
            }
        }
        for (auto const& beforeCell : beforeGenome) {
            auto matchingAfterCells = beforeGenome | std::views::filter([&beforeCell](auto const& afterCell) {
                auto beforeCellClone = beforeCell;
                auto afterCellClone = afterCell;
                beforeCellClone.cellFunction.reset();
                afterCellClone.cellFunction.reset();
                return beforeCellClone == afterCellClone;
            });
            if (matchingAfterCells.empty()) {
                return false;
            }
            if (beforeCell.getCellFunctionType() == CellFunction_Constructor || beforeCell.getCellFunctionType() == CellFunction_Injector) {
                auto matches = false;
                auto beforeSubGenome = beforeCell.getSubGenome();
                auto beforeIsMakeCopyGenome = beforeCell.isMakeGenomeCopy();
                for (auto const& afterCell : matchingAfterCells) {
                    auto afterIsMakeCopyGenome = afterCell.isMakeGenomeCopy();
                    if (beforeIsMakeCopyGenome && *beforeIsMakeCopyGenome && afterIsMakeCopyGenome && *afterIsMakeCopyGenome) {
                        return true;
                    }
                    auto afterSubGenome = afterCell.getSubGenome();
                    if (beforeSubGenome && afterSubGenome) {
                        matches |= compareInsertMutation(*beforeSubGenome, *afterSubGenome);
                    }
                }
                if (!matches) {
                    return false;
                }
            }
        }
        return true;
    }
};

TEST_F(MutationTests, dataMutation_startPos)
{
    auto genome = createGenomeWithMultipleCellsWithDifferentFunctions();
    int byteIndex = 0;

    auto data = DataDescription().addCells(
        {CellDescription().setId(1).setCellFunction(ConstructorDescription().setGenome(genome).setCurrentGenomePos(byteIndex)).setExecutionOrderNumber(0)});

    _simController->setSimulationData(data);
    for (int i = 0; i < 10000; ++i) {
        _simController->testOnly_mutate(1, MutationType::Data);
    }

    auto actualData = _simController->getSimulationData();
    auto actualCellById = getCellById(actualData);

    auto actualConstructor = std::get<ConstructorDescription>(*actualCellById.at(1).cellFunction);
    EXPECT_TRUE(compareDataMutation(genome, actualConstructor.genome));
    EXPECT_EQ(byteIndex, actualConstructor.currentGenomePos);
}

TEST_F(MutationTests, dataMutation_endPos)
{
    auto genome = createGenomeWithMultipleCellsWithDifferentFunctions();
    int byteIndex = toInt(genome.size());

    auto data = DataDescription().addCells(
        {CellDescription().setId(1).setCellFunction(ConstructorDescription().setGenome(genome).setCurrentGenomePos(byteIndex)).setExecutionOrderNumber(0)});

    _simController->setSimulationData(data);
    for (int i = 0; i < 10000; ++i) {
        _simController->testOnly_mutate(1, MutationType::Data);
    }

    auto actualData = _simController->getSimulationData();
    auto actualCellById = getCellById(actualData);

    auto actualConstructor = std::get<ConstructorDescription>(*actualCellById.at(1).cellFunction);
    EXPECT_TRUE(compareDataMutation(genome, actualConstructor.genome));
    EXPECT_EQ(byteIndex, actualConstructor.currentGenomePos);
}

TEST_F(MutationTests, dataMutation_invalidPos)
{
    auto genome = createGenomeWithMultipleCellsWithDifferentFunctions();
    int byteIndex = toInt(genome.size()) / 2;

    auto data = DataDescription().addCells(
        {CellDescription().setId(1).setCellFunction(ConstructorDescription().setGenome(genome).setCurrentGenomePos(byteIndex)).setExecutionOrderNumber(0)});

    _simController->setSimulationData(data);
    for (int i = 0; i < 10000; ++i) {
        _simController->testOnly_mutate(1, MutationType::Data);
    }

    auto actualData = _simController->getSimulationData();
    auto actualCellById = getCellById(actualData);

    auto actualConstructor = std::get<ConstructorDescription>(*actualCellById.at(1).cellFunction);
    EXPECT_TRUE(compareDataMutation(genome, actualConstructor.genome));
    EXPECT_EQ(byteIndex, actualConstructor.currentGenomePos);
}

TEST_F(MutationTests, neuronDataMutation)
{
    auto genome = createGenomeWithMultipleCellsWithDifferentFunctions();
    int byteIndex = 0;

    auto data = DataDescription().addCells(
        {CellDescription().setId(1).setCellFunction(ConstructorDescription().setGenome(genome).setCurrentGenomePos(byteIndex)).setExecutionOrderNumber(0)});

    _simController->setSimulationData(data);
    for (int i = 0; i < 10000; ++i) {
        _simController->testOnly_mutate(1, MutationType::NeuronData);
    }

    auto actualData = _simController->getSimulationData();
    auto actualCellById = getCellById(actualData);

    auto actualConstructor = std::get<ConstructorDescription>(*actualCellById.at(1).cellFunction);
    EXPECT_TRUE(compareNeuronDataMutation(genome, actualConstructor.genome));
    EXPECT_EQ(byteIndex, actualConstructor.currentGenomePos);
}

TEST_F(MutationTests, cellFunctionMutation)
{
    auto genome = createGenomeWithMultipleCellsWithDifferentFunctions();
    auto cellIndex = 7;
    int byteIndex = GenomeDescriptionConverter::convertCellIndexToByteIndex(genome, cellIndex);

    auto data = DataDescription().addCells(
        {CellDescription().setId(1).setCellFunction(ConstructorDescription().setGenome(genome).setCurrentGenomePos(byteIndex)).setExecutionOrderNumber(0)});

    _simController->setSimulationData(data);
    for (int i = 0; i < 10000; ++i) {
        _simController->testOnly_mutate(1, MutationType::CellFunction);
    }

    auto actualData = _simController->getSimulationData();
    auto actualCellById = getCellById(actualData);

    auto actualConstructor = std::get<ConstructorDescription>(*actualCellById.at(1).cellFunction);
    EXPECT_TRUE(compareCellFunctionMutation(genome, actualConstructor.genome));
    auto actualCellIndex = GenomeDescriptionConverter::convertByteIndexToCellIndex(actualConstructor.genome, actualConstructor.currentGenomePos);
    EXPECT_EQ(cellIndex, actualCellIndex);
}

TEST_F(MutationTests, insertMutation_emptyGenome)
{
    auto genome = createGenomeWithMultipleCellsWithDifferentFunctions();

    auto cellColor = 3;
    auto data = DataDescription().addCells(
        {CellDescription().setId(1).setCellFunction(ConstructorDescription()).setExecutionOrderNumber(0).setColor(cellColor)});

    _simController->setSimulationData(data);
    _simController->testOnly_mutate(1, MutationType::Insertion);

    auto actualData = _simController->getSimulationData();
    auto actualCellById = getCellById(actualData);

    auto actualConstructor = std::get<ConstructorDescription>(*actualCellById.at(1).cellFunction);

    auto actualGenomeDescription = GenomeDescriptionConverter::convertBytesToDescription(actualConstructor.genome, _parameters);
    EXPECT_EQ(1, actualGenomeDescription.size());
    EXPECT_EQ(cellColor, actualGenomeDescription.front().color);
}

TEST_F(MutationTests, insertMutation)
{
    auto genome = createGenomeWithMultipleCellsWithDifferentFunctions();

    auto data = DataDescription().addCells(
        {CellDescription().setId(1).setCellFunction(ConstructorDescription().setGenome(genome).setCurrentGenomePos(0)).setExecutionOrderNumber(0)});

    _simController->setSimulationData(data);
    for (int i = 0; i < 10000; ++i) {
        _simController->testOnly_mutate(1, MutationType::Insertion);
    }

    auto actualData = _simController->getSimulationData();
    auto actualCellById = getCellById(actualData);

    auto actualConstructor = std::get<ConstructorDescription>(*actualCellById.at(1).cellFunction);
    EXPECT_TRUE(compareInsertMutation(genome, actualConstructor.genome));
    EXPECT_EQ(0, actualConstructor.currentGenomePos);
}
