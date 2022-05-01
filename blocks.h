#ifndef BLOCKS_H
#define BLOCKS_H

/* Здесь заданы структуры отображений и размеры всех блоков тетриса */

const bool displays[4][7][4][4] = {
        {
            {
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0}
            }, {
                {1,1,0,0},
                {1,1,0,0}
            }, {
                {1,0,0,0},
                {1,0,0,0},
                {1,1,0,0}
            }, {
                {0,1,0,0},
                {0,1,0,0},
                {1,1,0,0}
            }, {
                {0,1,1,0},
                {1,1,0,0}
            }, {
                {1,1,0,0},
                {0,1,1,0}
            }, {
                {0,1,0,0},
                {1,1,1,0}
            }
        }, {
            {
                {1,1,1,1}
            }, {
                {1,1,0,0},
                {1,1,0,0}
            }, {
                {1,1,1,0},
                {1,0,0,0}
            }, {
                {1,0,0,0},
                {1,1,1,0}
            }, {
                {1,0,0,0},
                {1,1,0,0},
                {0,1,0,0}
            }, {
                {0,1,0,0},
                {1,1,0,0},
                {1,0,0,0}
            }, {
                {1,0,0,0},
                {1,1,0,0},
                {1,0,0,0}
            }
        }, {
            {
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0},
                {1,0,0,0}
            }, {
                {1,1,0,0},
                {1,1,0,0}
            }, {
                {1,1,0,0},
                {0,1,0,0},
                {0,1,0,0}
            }, {
                {1,1,0,0},
                {1,0,0,0},
                {1,0,0,0}
            }, {
                {0,1,1,0},
                {1,1,0,0}
            }, {
                {1,1,0,0},
                {0,1,1,0}
            }, {
                {1,1,1,0},
                {0,1,0,0}
            }
        }, {
            {
                {1,1,1,1}
            }, {
                {1,1,0,0},
                {1,1,0,0}
            }, {
                {0,0,1,0},
                {1,1,1,0}
            }, {
                {1,1,1,0},
                {0,0,1,0}
            }, {
                {1,0,0,0},
                {1,1,0,0},
                {0,1,0,0}
            }, {
                {0,1,0,0},
                {1,1,0,0},
                {1,0,0,0}
            }, {
                {0,1,0,0},
                {1,1,0,0},
                {0,1,0,0}
            }
        }
};

const int sizes[4][7][2] = {
        {
            {1,4},{2,2},{2,3},{2,3},{3,2},{3,2},{3,2}
        }, {
            {4,1},{2,2},{3,2},{3,2},{2,3},{2,3},{2,3}
        }, {
            {1,4},{2,2},{2,3},{2,3},{3,2},{3,2},{3,2}
        }, {
            {4,1},{2,2},{3,2},{3,2},{2,3},{2,3},{2,3}
        }
};

#endif // BLOCKS_H