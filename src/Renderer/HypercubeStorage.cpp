#include "Cellnta/Renderer/HypercubeStorage.h"

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

#define HYPERCUBE_DEBUG_PRINT

#define CHECK_POLYGON_INDEXES_IN_RANGE(pol, min, max)                          \
  do {                                                                         \
    assert(min <= (pol)[0] && (pol)[0] < max && "Polygon index not in range"); \
    assert(min <= (pol)[1] && (pol)[1] < max && "Polygon index not in range"); \
    assert(min <= (pol)[2] && (pol)[2] < max && "Polygon index not in range"); \
  } while (0)

#define CHECK_POLYGON_VALID_INDEXES(pol, dim) \
  CHECK_POLYGON_INDEXES_IN_RANGE(pol, 0, GetVerticesCount(dim))

#define CHECK_POLYGON_IS_NOT_LINE(pol)                   \
  do {                                                   \
    assert((pol)[0] != (pol)[1] && "Polygon is a line"); \
    assert((pol)[0] != (pol)[2] && "Polygon is a line"); \
    assert((pol)[1] != (pol)[2] && "Polygon is a line"); \
  } while (0)

#define CHECK_POLYGON_CORRECTNESS(pol, dim)  \
  do {                                       \
    CHECK_POLYGON_VALID_INDEXES((pol), dim); \
    CHECK_POLYGON_IS_NOT_LINE((pol));        \
  } while (0)

#define CHECK_FACE_CORRECTNESS(face, dim)       \
  do {                                          \
    CHECK_POLYGON_CORRECTNESS((face) + 0, dim); \
    CHECK_POLYGON_CORRECTNESS((face) + 3, dim); \
  } while (0)

#define CHECK_FACE_OUT_OF_BOUNDS(faceStart, dataStart, dataEnd)               \
  do {                                                                        \
    assert((faceStart) + 0 >= (dataStart) && "Data is before indices start"); \
    assert((faceStart) + 6 <= (dataEnd) && "Data is before indices end");     \
  } while (0)

using namespace Cellnta;

void HypercubeStorage::Restore() {
  CELLNTA_PROFILE;

  m_pnt = m_origPnt;
}

void HypercubeStorage::GenerateCube(int dim, Point size, CubeMode mode) {
  CELLNTA_PROFILE;

  CELLNTA_LOG_DEBUG("Trying to generate hypercube");

  if (dim != m_d) {
    if (size == (Point)-1)
      size = GetSize();
    if (mode == CubeMode::NONE)
      mode = GetMode();
  } else {
    if (size == GetSize())
      size = -1;
    if (mode == GetMode())
      mode = CubeMode::NONE;
  }

  m_d = dim;

  if (size != -1) {
    SetSize(size);
  }

  SetMode(mode);
}

void HypercubeStorage::SetDimension(int dim) {
  CELLNTA_PROFILE;

  m_d = dim;
  SetSize(GetSize());
  SetMode(GetMode());
}

void HypercubeStorage::SetSize(Point size) {
  CELLNTA_PROFILE;

  if (size < 0)
    return;

  m_cubeSize = size;
  GenerateVertices();

  m_needUpdatePoints = true;
}

void HypercubeStorage::SetMode(CubeMode mode) {
  CELLNTA_PROFILE;

  if (mode == CubeMode::NONE)
    return;

  if (m_d == 1 && mode == CubeMode::POLYGON)
    mode = CubeMode::WIREFRAME;
  if (m_d == 0 && (mode == CubeMode::POLYGON || mode == CubeMode::WIREFRAME))
    mode = CubeMode::POINTS;

  m_mode = mode;
  switch (mode) {
    case CubeMode::POLYGON: GenerateIndicesPolygon(); break;
    case CubeMode::WIREFRAME: GenerateIndicesWireframe(); break;
    case CubeMode::POINTS: GenerateIndicesPoints(); break;
    default: break;
  }

  m_needUpdateIndices = true;
}

void HypercubeStorage::GenerateVertices() {
  CELLNTA_PROFILE;

  m_pnt.resize(GetPointsSize());

  const uint32_t halfVert = GetVerticesCount() >> 1;
  Point* data = m_pnt.data();
  Point* sec_data = m_pnt.data() + halfVert * GetVertexSize();
  // TODO: Better use dynamic bitset
  for (uint32_t bitset = 0; bitset < halfVert; ++bitset) {
    uint32_t j = 0;
    uint64_t pos = bitset * GetVertexSize();
    for (; j < (uint32_t)m_d - 1; ++j, ++pos) {
      // Check j bit
      if (bitset & ((uint32_t)1 << j))
        data[pos] = sec_data[pos] = +m_cubeSize;
      else
        data[pos] = sec_data[pos] = -m_cubeSize;
    }
    data[pos] = -m_cubeSize;
    data[pos + 1] = 1.0f;
    sec_data[pos] = +m_cubeSize;
    sec_data[pos + 1] = 1.0f;
  }

  m_pnt.shrink_to_fit();
  m_origPnt = m_pnt;

#ifdef HYPERCUBE_DEBUG_PRINT
  CELLNTA_LOG_DEBUG("Points:");
  for (size_t i = 0; i < m_pnt.size() / GetVertexSize(); ++i) {
    int offset = i * GetVertexSize();
    auto start = m_pnt.begin() + offset;
    auto end = start + GetVertexSize();
    CELLNTA_LOG_DEBUG("{}: {:+f}", i, fmt::join(start, end, ", "));
  }
#endif  // HYPERCUBE_DEBUG_PRINT
}

void HypercubeStorage::GenerateIndicesPolygon() {
  CELLNTA_PROFILE;

  /****************************************
  Monospace font reqired.
  If the figures are still broken - use Iosevka Nerd Font Mono.

  Terms:
    dim[s]: Shorten from 'dimension[s]'
    N: Any dimension. Also it can mean - sign of new axis in dimension
      i.e in 4d its 'w' sign, since 4d has 'xyzw' axises
    N-cube: Shorten from 'N-dimension hypercube'

  Since my English is very poor - if you see any grammar error,
  please create pull request to fix it
  ****************************************/

  // clang-format on
  if (m_d <= 1) {
    CELLNTA_LOG_ERROR(
        "Unable to generate polygon indices: dimension must be >= 2, "
        "but actual: '{}'",
        m_d);
    return;
  }

  // Enable when algorithm not fully generate indices
  // m_ind.clear();
  m_ind.resize(GetFacesCount() * 6, 0);
  const Ind* dataStart = m_ind.data();
  const Ind* dataEnd = m_ind.data() + m_ind.size();
  Ind* data = m_ind.data();

  for (int currDim = 2; currDim <= m_d; ++currDim) {
    // clang-format off

  /****************************************
    Lets start from generating `3-cube`.
    Since `2-cube` is already generated, lets make second instance of `2-cube`
    but in new dimension.

    We already have square:

    ^y      z
    ╎      ⟋
    ╎     ⟋
    ╎    ⟋
    ╎   ⟋ 1━━━━━━━3
    ╎  ⟋  ┃       ┃
    ╎ ⟋   ┃       ┃ <------ Generated from previous
    ╎⟋    ┃       ┃         iterations
    └-----0━━━━━━━2-->x

    So lets do a copy of square,
    but with offset in `z-axis`.

    ∧y      z
    ╎      ⟋  5━━━━━━━7
    ╎     ⟋   ┃       ┃
    ╎    ⟋    ┃       ┃ <---- Copy with offset in new
    ╎   ⟋ 1───╂───3   ┃       dimension
    ╎  ⟋  │   4━━━┿━━━6
    ╎ ⟋   │       │
    ╎⟋    │       │
    └-----0───────2-->x

  Summary:
    Make a copy of (N - 1)-cube in `N-axis`

  ****************************************/

    // clang-format on

    const int prevDimCount = data - dataStart;
    const int newAxisOffset = GetVerticesCount(currDim - 1);
    while (data < dataStart + prevDimCount * 2) {
      CHECK_FACE_OUT_OF_BOUNDS(data, dataStart, dataEnd);

      const Ind* prevDim = data - prevDimCount;
      *data = *prevDim + newAxisOffset;

      ++data;
    }

    // clang-format off

  /****************************************
    Now we need to connect two hypercubes.

    ^y
    ╎         5───────7
    ╎        ⟋│      ⟋│
    ╎      ⟋  │    ⟋  │
    ╎     1───┼───3   │
    ╎     │   4───┼───6
    ╎     │  ⟋    │  ⟋
    ╎     │⟋      │⟋
    └-----0───────2-->x

    But how do that?

    First of all, we need to know how many faces we need to generate.
    For 3d - 4 faces

    Also look at planes in 2d and 3d
      2d: 1  - xy       planes
      3d: 3  - xy,xz,yz planes

    As you can see - 3d adds two new planes: `xz` and `yz`
    So we need to generate new planes two times (4 total / 2 new = 2 times)
    Let's call the generating of new planes: `jumps`

    Summary:
      We need to generate some count of new planes
      Formula:
        newPlanesCount(N) = GetPlanesCount(N) - GetPlanesCount(N - 1)
        or optimized version:
        newPlanesCount(N) = N - 1

      Also need to generate new planes several times
      Formula:
        totalJumps(N) = GetRemaindingFacesCount(N)
        or:
        totalJumps(N) = 2^(N - 2)


    ## Generating new planes around 0 index
    Forget about `jumps` and focus only about how generate new planes.

    Since we need to generate 2 faces around 0 index,
    result need be as:

    ^y      z
    ╎      ⟋  5
    ╎     ⟋  ╱│
    ╎    ⟋ ╱░░│
    ╎   ⟋ 1░░░│
    ╎  ⟋  │░░░4───────6
    ╎ ⟋   │░░╱░░░░░░░╱
    ╎⟋    │╱░░░░░░░╱
    └-----0───────2-->x

    First face: 0,1,4,5
    Second face: 0,2,4,6
    So what common in this two faces?
    1) Both faces contain 0-4 line.
      Let's call this line as `common edge`.

    2) First face has 1 index in 0-1 line,
      and second face has 2 index in  0-2 line.
      Both indexes - power of two.
      Let's call this indexes as `middle point`.

    3) Unfortunaly 0-5 and 0-6 is not a line.
      But it can be determined as:
        'middle point' + 4 - 0
      For first face: 1 + 4 - 0 = 5
      For second face: 2 + 4 - 0 = 6

    ^y      z                     ^y      z
    ╎      ⟋  5 <━━ Total point   ╎      ⟋     Total point
    ╎     ⟋  ╱│     (4 + 1 - 0)   ╎     ⟋      (4 + 2 - 0)
    ╎    ⟋ ╱  │                   ╎ Common edge     ┃
    ╎   ⟋ 1 <━┿━━━━ Mid point     ╎   ⟋  ┃          ⌄
    ╎  ⟋  │   4     (0 + 2^0)     ╎  ⟋   ┃  4───────6
    ╎ ⟋   │  ╱                    ╎ ⟋    ⌄ /       /
    ╎⟋    │╱ <━━━━━ Common edge   ╎⟋     /       /    Mid point
    └-----0------------>x         └-----0───────2 <━━ (0 + 2^1)

    Now we know how to generate half of 3-cube.

    Summary:
      We need to generate newPlanesCount(n) faces

      Every face contain 0-v `common edge`,
      where v = 'start of common edge' + GetVerticesCount(N - 1)

      Every face contain 2^('index of current face,
      starting from 0') `middle point`,

      Every face contain total point that can be determined as:
      'middle point' + 'end of common edge' - 'start of common edge'


    ## Generating new planes around jump
    Since for 3d-cube we need make 2 jumps
    and one we already make (with 0-4 commond edge)
    So we need to make last jump
    You might be already guesed that this jump would be with 3-7 common edge

    ^y      z
    ╎      ⟋  5───────7
    ╎     ⟋  ╱░░░░░░░╱│
    ╎    ⟋ ╱░░░░░░░╱░░│
    ╎   ⟋ 1───────3░░░│
    ╎  ⟋          │░░░6
    ╎ ⟋           │░░╱
    ╎⟋            │╱
    └-------------2-->x

    But how to come to this common edge?
    3 - its total point of 0 index in 2d-cube
    or its opposite of 0 in 2d-cube
    or even middle point of middle point

    Since 3d-cube is very easy to show how algorithm work
    lets introduce 4d-cube

    Okay, so in 4d-cube we need to generate 4 faces in one jump
    First off all we need to generate faces around `0-8 common edge`

    `Some points and edges is not will be shown`

            15──────────────────13
           / ⟍                 ⟋ │
         /     ⟍             ⟋   │
       11───────────────────9    │
       │ ⟍        7─────── ⟋│    │
       │   ⟍     /│      ⟋░░│    │
       │     ⟍ /  │    ⟋░░░░│    │
       │      3───┼───1░░░░░│    │
       │      │   6───┼░░░░░│    │
       │      │  /    │░░░▒▒│    │
       │      │/      │░▒▒▒▒│⟍   │
       │      2───────0▒▒▒▒▒│░░⟍ │
       │     ⟋░░░░░░░░░⟍▒▒▒▒│░░░░12
       │   ⟋░░░░░░░░░░░░░⟍▒▒│░░/
       │ ⟋░░░░░░░░░░░░░░░░░⟍│/
       10───────────────────8

    So our task is to choose the right jumps that do not create any overlaps.
    Only way to do this - use these jump points: 0, 3, 6, 5

    To understand this - use directions:
      We starting from 0 index.
      0) From 0 idx moving to xy direction
      1) From 3 idx moving to zy direction
      2) From 6 idx movint to xy direction
      And 5 idx is last

    Okay cool, but we didn't get any jump points!
    Its easy to change.
    Now replace sign of axis to dimension number in power of two
    (Yes `x` and `y` are swapped. `y` - is zero dimension)
    And we got:
       We starting from 0 index.
       0) From 0 idx moving to (2^1) + (2^0): 0 + 2 + 1 = 3 index
       1) From 3 idx moving to (2^2) + (2^0): 3 + 4 - 1 = 6 index
       2) From 6 idx movint to (2^1) - (2^0): 6 - 2 + 1 = 5 index
       And 5 idx is last

    Hey, but why sometimes used + and sometimes -?
    I don't know, just found pattern, so look at the table:

                              Jump number
            0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 ...
        1:  +  -  +  -  +  -  +  -  +  -  +  -  +  -  +  -  +  ...
      D 2:  +  -  -  +  +  -  -  +  +  -  -  +  +  -  -  +  +  ...
      i 4:  +  +  -  -  -  -  +  +  +  +  -  -  -  -  +  +  +  ...
      m 8:  +  +  +  +  -  -  -  -  -  -  -  -  +  +  +  +  +  ...
        16: +  +  +  +  +  +  +  +  -  -  -  -  -  -  -  -  -  ...
        ...

    I hope you see pattern.

    So what we did here?
    Techically, we found the neighbor of the index in Nd
    and then we found the neighbor of the neighbor but in 2d
    And just made sum of these values

    So that all?
    Nope.
    We a have a one bug.

    When Im introduce `middle point' i say that they can be only pow of two.
    But this is not true.
    Its valid only for 0 jump
    You can see this error for example in second jump, in 3-11 common edge

            15──────────────────13
           / \                 / │
         /     \             /   │
       11───────────────────9    │
       │ ⟍        7───────5 │    │
       │   ⟍     ⟋│      /│ │    │
       │     ⟍ ⟋  │    /  │ │    │
       │      3━━━┿━━━1   │ │    │
       │      ┃   6───┼───4 │    │
       │      ┃  /    │  / \│    │
       │      ┃/      │/    │\   │
       │      2───────0     │  \ │
       │     /         \    │   12
       │   /             \  │  /
       │ /                 \│/
       10───────────────────8

    As you can see we need connect 2, 1 and 7 middle points
    But if we use only power of two, we got: 1, 2 and 4
    So how we can fix that?

    So our task its find neighbor of start edge
    Formula:
      realMidPoint = `edge start` (+ or -) `middle point`
    How to decide sign?
    Use table above

    And that all!

    #### Begin of Higher dimension representation

    All higher than 3-cubes will be shown as:
     1─────────3
     │╲       ╱│      0-1-2-3 face - forward
     │ 5─────7 │      4-5-6-7 face - backward
     │ │     │ │
     │ │     │ │
     │ 4─────6 │
     │╱       ╲│
     0─────────2

    This may lead to strange results in >= 5d.
    For example, in 5d some faces in our perspective will show up as lines.
    This happens because 4d overlaps 5d
    But keep in mind that it are true faces

    #### End of Higher dimension representation

    Just leave 5-cube here:

             31━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━29
            / \                                               /┃
          /     \                                           /  ┃
        27━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━25    ┃
        ┃\         23━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━21 ┃    ┃
        ┃  \      ⟋ ⟍                                 ⟋ ┃ ┃    ┃
        ┃    \  ⟋     ⟍                             ⟋   ┃ ┃    ┃
        ┃     19━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━17    ┃ ┃    ┃
        ┃     ┃⟍          ⟍                        ┃    ┃ ┃    ┃
        ┃     ┃  ⟍         15──────────────────13  ┃    ┃ ┃    ┃
        ┃     ┃    ⟍      / \                 / │  ┃    ┃ ┃    ┃
        ┃     ┃      ⟍  /     \             /   │  ┃    ┃ ┃    ┃
        ┃     ┃       11───────────────────9    │  ┃    ┃ ┃    ┃
        ┃     ┃       │ \        7───────5 │    │  ┃    ┃ ┃    ┃
        ┃     ┃       │   \     /│      /│ │    │  ┃    ┃ ┃    ┃
        ┃     ┃       │     \ /  │    /  │ │    │  ┃    ┃ ┃    ┃
        ┃     ┃       │      3───┼───1   │ │    │  ┃    ┃ ┃    ┃
        ┃     ┃       │      │   6───┼───4 │    │  ┃    ┃ ┃    ┃
        ┃     ┃       │      │  /    │  / \│    │  ┃    ┃ ┃    ┃
        ┃     ┃       │      │/      │/    │\   │  ┃    ┃ ┃    ┃
        ┃     ┃       │      2───────0     │  \ │  ┃    ┃ ┃    ┃
        ┃     ┃       │     /         \    │   12  ┃    ┃ ┃    ┃
        ┃     ┃       │   /             \  │  /  ⟍ ┃    ┃ ┃    ┃
        ┃     ┃       │ /                 \│/      ┃    ┃ ┃    ┃
        ┃     ┃       10───────────────────8       ┃ ⟍  ┃ ┃    ┃
        ┃     ┃      ⟋                      ⟍      ┃   ⟍┃ ┃    ┃
        ┃     ┃    ⟋                          ⟍    ┃   20 ┃    ┃
        ┃     ┃  ⟋                              ⟍  ┃  ⟋  \┃    ┃
        ┃     ┃⟋                                  ⟍┃⟋     ┃\   ┃
        ┃     18━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━16      ┃  \ ┃
        ┃    /                                      \     ┃   28
        ┃  /                                          \   ┃  /
        ┃/                                              \ ┃/
        26━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━24

    ****************************************/

    // clang-format on
    auto GetMiddlePointSign = [&](Ind n, size_t dimPow) -> int {
      assert((dimPow & (dimPow - 1)) == 0 && "dimPow must be power of two");

      if (dimPow == 1) {
        return ((n & 1) == 0) ? 1 : -1;
      }

      const size_t group = n / dimPow;
      const size_t idxInGroup = n % dimPow;

      int sign = (idxInGroup >= (dimPow >> 1)) ? -1 : 1;
      if ((group & 1) != 0)
        sign = -sign;
      return sign;
    };

    size_t edgeStart = 0;
    size_t edgeEnd = GetVerticesCount(currDim - 1);

    size_t totalJumps = GetRemaindingFacesCount(currDim);
    for (size_t currJump = 0; currJump < totalJumps; ++currJump) {
      size_t currPlane = 0;
      size_t midPoint = 1;
      while (currPlane < currDim - 1) {
        assert(midPoint != edgeStart &&
               "Middle point cannot be start of common edge");

        int sign = GetMiddlePointSign(currJump, midPoint);
        size_t realMidPoint = edgeStart + ((int64_t)midPoint * sign);
        size_t totalPoint = edgeEnd + realMidPoint - edgeStart;

        // clang-format off
        CHECK_FACE_OUT_OF_BOUNDS(data, dataStart, dataEnd);
        data[0] = edgeStart; data[1] = realMidPoint; data[2] = totalPoint;
        data[3] = edgeStart; data[4] = edgeEnd;      data[5] = totalPoint;
        CHECK_FACE_CORRECTNESS(data, currDim);
        data += 6;
        // clang-format on

        ++currPlane;
        midPoint <<= 1;
      }

      // https://oeis.org/A171977: -(-n XOR n)
      size_t jumpNeighbor = -(-(currJump + 1) ^ (currJump + 1));
      int jumpNeiSign = GetMiddlePointSign(currJump, jumpNeighbor);
      int jumpIn2d = GetMiddlePointSign(currJump, 1);

      edgeStart += (jumpNeighbor * jumpNeiSign) + jumpIn2d;
      edgeEnd = edgeStart + GetVerticesCount(currDim - 1);
    }
  }

  m_ind.shrink_to_fit();

#ifdef HYPERCUBE_DEBUG_PRINT
  const size_t printCount = data - dataStart;
  const size_t mustCount = dataEnd - dataStart;

  CELLNTA_LOG_DEBUG("Polygon: (faces={})", GetFacesCount());
  if (printCount != mustCount) {
    CELLNTA_LOG_WARN(
        "Not all indices generated. Must be '{}', but actual: '{}'", mustCount,
        printCount);
  }

  for (size_t i = 0; i < printCount; i += 6) {
    CELLNTA_LOG_DEBUG("Face: {}", i / 6);
    for (int j = 0; j < 2; ++j) {
      constexpr int offset = 3;
      auto start = m_ind.begin() + i + j * offset;
      auto end = start + offset;
      CELLNTA_LOG_DEBUG("{}", fmt::join(start, end, ", "));
    }
  }
#endif  // HYPERCUBE_DEBUG_PRINT
}

void HypercubeStorage::GenerateIndicesWireframe() {
  CELLNTA_PROFILE;

  // m_ind.clear();
  m_ind.resize(GetVerticesCount() * m_d);
  // or m_ind.resize(GetFacesCount() * 4);

  Ind* data = m_ind.data();

  // https://stackoverflow.com/questions/51877294/connecting-points-in-n-dimensional-hyper-cube
  for (int currDim = 0; currDim < m_d; currDim++) {
    const int nVert = ((int)1) << currDim;
    const Ind* indicesEnd = data;

    // copy + shift previous edges
    Ind* old_data = m_ind.data();
    while (old_data < indicesEnd) {
      assert(data < m_ind.data() + m_ind.size() && "Out of range");
      assert(old_data < m_ind.data() + m_ind.size() && "Out of range");

      data[0] = old_data[0] + nVert;
      data[1] = old_data[1] + nVert;
      data += 2;
      old_data += 2;
    }

    // create new edges to join cube copies
    for (int i = 0; i < nVert; ++i) {
      assert(data < m_ind.data() + m_ind.size() && "Out of range");

      data[0] = i;
      data[1] = nVert + i;
      data += 2;
    }
  }

  m_ind.shrink_to_fit();

#ifdef HYPERCUBE_DEBUG_PRINT
  CELLNTA_LOG_DEBUG("Wire frame:");
  for (size_t i = 0; i < m_ind.size(); i += 2)
    CELLNTA_LOG_DEBUG("{}, {}", m_ind[i + 0], m_ind[i + 1]);
#endif
}

void HypercubeStorage::GenerateIndicesPoints() {
  m_ind.resize(GetVerticesCount());
  for (int i = 0; i < GetVerticesCount(); ++i)
    m_ind[i] = i;
}

int HypercubeStorage::GetPlanesCount(int dim) const {
  if (dim < 0)
    return 0;
  // Optimized binomial coefficient
  // where n=dim, k=2
  return dim * (dim - 1) >> 1;
}

int HypercubeStorage::GetRemaindingFacesCount(int dim) const {
  return ((int)1) << (dim - 2);
}

int HypercubeStorage::GetFacesCount(int dim) const {
  // https://oeis.org/A001788
  if (dim < 0)
    return 0;
  return GetPlanesCount(dim) * GetRemaindingFacesCount(dim);
}

int HypercubeStorage::GetEdgesCount(int dim) const {
  // https://oeis.org/A001787
  if (dim < 0)
    return 0;
  return dim * GetVerticesCount(dim - 1);
}

int HypercubeStorage::GetVerticesCount(int dim) const {
  if (dim < 0)
    return 0;
  return ((int)1) << dim;
}

#undef HYPERCUBE_DEBUG_PRINT
#undef HYPERCUBE_VALIDATE_FACE
