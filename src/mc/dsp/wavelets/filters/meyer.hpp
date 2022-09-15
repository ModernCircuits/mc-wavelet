#pragma once

namespace mc::dsp {

constexpr float meyer[102] = {
    -0.000001509740857, 0.000001278766757,  0.000000449585560,
    -0.000002096568870, 0.000001723223554,  0.000000698082276,
    -0.000002879408033, 0.000002383148395,  0.000000982515602,
    -0.000004217789186, 0.000003353501538,  0.000001674721859,
    -0.000006034501342, 0.000004837555802,  0.000002402288023,
    -0.000009556309846, 0.000007216527695,  0.000004849078300,
    -0.000014206928581, 0.000010503914271,  0.000006187580298,
    -0.000024438005846, 0.000020106387691,  0.000014993523600,
    -0.000046428764284, 0.000032341311914,  0.000037409665760,
    -0.000102779005085, 0.000024461956845,  0.000149713515389,
    -0.000075592870255, -0.000139913148217, -0.000093512893880,
    0.000161189819725,  0.000859500213762,  -0.000578185795273,
    -0.002702168733939, 0.002194775336459,  0.006045510596456,
    -0.006386728618548, -0.011044641900539, 0.015250913158586,
    0.017403888210177,  -0.032094063354505, -0.024321783959519,
    0.063667300884468,  0.030621243943425,  -0.132696615358862,
    -0.035048287390595, 0.444095030766529,  0.743751004903787,
    0.444095030766529,  -0.035048287390595, -0.132696615358862,
    0.030621243943425,  0.063667300884468,  -0.024321783959519,
    -0.032094063354505, 0.017403888210177,  0.015250913158586,
    -0.011044641900539, -0.006386728618548, 0.006045510596456,
    0.002194775336459,  -0.002702168733939, -0.000578185795273,
    0.000859500213762,  0.000161189819725,  -0.000093512893880,
    -0.000139913148217, -0.000075592870255, 0.000149713515389,
    0.000024461956845,  -0.000102779005085, 0.000037409665760,
    0.000032341311914,  -0.000046428764284, 0.000014993523600,
    0.000020106387691,  -0.000024438005846, 0.000006187580298,
    0.000010503914271,  -0.000014206928581, 0.000004849078300,
    0.000007216527695,  -0.000009556309846, 0.000002402288023,
    0.000004837555802,  -0.000006034501342, 0.000001674721859,
    0.000003353501538,  -0.000004217789186, 0.000000982515602,
    0.000002383148395,  -0.000002879408033, 0.000000698082276,
    0.000001723223554,  -0.000002096568870, 0.000000449585560,
    0.000001278766757,  -0.000001509740857, 0.0,
};
}  // namespace mc::dsp
