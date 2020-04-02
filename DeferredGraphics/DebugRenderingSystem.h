#pragma once
#include "Scene.h"
#include "AABB.h"
#include "BoundingSphere.h"
#include "BoundingVolumeHierarchy.h"
#include "Octree.h"


class DebugRenderingSystem
{
  //projection and view matrix ID
  GLuint projectionMatrixID;
  GLuint viewMatrixID;

  //AABB Bounding Box data
  GLuint boundingBoxVAOID = 0;
  GLuint boundingBoxVBOID;
  GLuint boundingBoxIBOID;
  std::vector<glm::uint> cubeIndices =
  {
    0,1,
    1,2,
    2,3,
    3,0,
    0,4,
    4,5,
    5,6,
    6,7,
    3,7,
    7,4,
    1,5,
    2,6
  };

  //Bounding Sphere data
  GLuint boundingSphereVAOID = 0;
  GLuint boundingSphereVBOID;
  GLuint boundingSphereIBOID;
  GLuint SphereObjectToWorld;
  std::vector<glm::uint> sphereIndices = { 293,
349,
385,
293,
385,
363,
292,
348,
349,
292,
349,
293,
291,
347,
348,
291,
348,
292,
254,
346,
347,
254,
347,
291,
290,
293,
363,
290,
363,
362,
289,
292,
293,
289,
293,
290,
288,
291,
292,
288,
292,
289,
253,
254,
291,
253,
291,
288,
287,
290,
362,
287,
362,
361,
286,
289,
290,
286,
290,
287,
285,
288,
289,
285,
289,
286,
252,
253,
288,
252,
288,
285,
257,
287,
361,
257,
361,
360,
256,
286,
287,
256,
287,
257,
255,
285,
286,
255,
286,
256,
245,
252,
285,
245,
285,
255,
284,
307,
381,
284,
381,
343,
283,
306,
307,
283,
307,
284,
282,
305,
306,
282,
306,
283,
251,
304,
305,
251,
305,
282,
281,
284,
343,
281,
343,
344,
280,
283,
284,
280,
284,
281,
279,
282,
283,
279,
283,
280,
250,
251,
282,
250,
282,
279,
278,
281,
344,
278,
344,
345,
277,
280,
281,
277,
281,
278,
276,
279,
280,
276,
280,
277,
249,
250,
279,
249,
279,
276,
254,
278,
345,
254,
345,
346,
253,
277,
278,
253,
278,
254,
252,
276,
277,
252,
277,
253,
245,
249,
276,
245,
276,
252,
275,
308,
378,
275,
378,
301,
274,
309,
308,
274,
308,
275,
273,
310,
309,
273,
309,
274,
248,
311,
310,
248,
310,
273,
272,
275,
301,
272,
301,
302,
271,
274,
275,
271,
275,
272,
270,
273,
274,
270,
274,
271,
247,
248,
273,
247,
273,
270,
269,
272,
302,
269,
302,
303,
268,
271,
272,
268,
272,
269,
267,
270,
271,
267,
271,
268,
246,
247,
270,
246,
270,
267,
251,
269,
303,
251,
303,
304,
250,
268,
269,
250,
269,
251,
249,
267,
268,
249,
268,
250,
245,
246,
267,
245,
267,
249,
266,
357,
382,
266,
382,
314,
265,
358,
357,
265,
357,
266,
264,
359,
358,
264,
358,
265,
257,
360,
359,
257,
359,
264,
263,
266,
314,
263,
314,
313,
262,
265,
266,
262,
266,
263,
261,
264,
265,
261,
265,
262,
256,
257,
264,
256,
264,
261,
260,
263,
313,
260,
313,
312,
259,
262,
263,
259,
263,
260,
258,
261,
262,
258,
262,
259,
255,
256,
261,
255,
261,
258,
248,
260,
312,
248,
312,
311,
247,
259,
260,
247,
260,
248,
246,
258,
259,
246,
259,
247,
245,
255,
258,
245,
258,
246,
244,
343,
381,
244,
381,
335,
243,
344,
343,
243,
343,
244,
242,
345,
344,
242,
344,
243,
205,
346,
345,
205,
345,
242,
241,
244,
335,
241,
335,
334,
240,
243,
244,
240,
244,
241,
239,
242,
243,
239,
243,
240,
204,
205,
242,
204,
242,
239,
238,
241,
334,
238,
334,
333,
237,
240,
241,
237,
241,
238,
236,
239,
240,
236,
240,
237,
203,
204,
239,
203,
239,
236,
208,
238,
333,
208,
333,
332,
207,
237,
238,
207,
238,
208,
206,
236,
237,
206,
237,
207,
196,
203,
236,
196,
236,
206,
235,
377,
385,
235,
385,
349,
234,
376,
377,
234,
377,
235,
233,
375,
376,
233,
376,
234,
202,
374,
375,
202,
375,
233,
232,
235,
349,
232,
349,
348,
231,
234,
235,
231,
235,
232,
230,
233,
234,
230,
234,
231,
201,
202,
233,
201,
233,
230,
229,
232,
348,
229,
348,
347,
228,
231,
232,
228,
232,
229,
227,
230,
231,
227,
231,
228,
200,
201,
230,
200,
230,
227,
205,
229,
347,
205,
347,
346,
204,
228,
229,
204,
229,
205,
203,
227,
228,
203,
228,
204,
196,
200,
227,
196,
227,
203,
226,
342,
384,
226,
384,
371,
225,
341,
342,
225,
342,
226,
224,
340,
341,
224,
341,
225,
199,
339,
340,
199,
340,
224,
223,
226,
371,
223,
371,
372,
222,
225,
226,
222,
226,
223,
221,
224,
225,
221,
225,
222,
198,
199,
224,
198,
224,
221,
220,
223,
372,
220,
372,
373,
219,
222,
223,
219,
223,
220,
218,
221,
222,
218,
222,
219,
197,
198,
221,
197,
221,
218,
202,
220,
373,
202,
373,
374,
201,
219,
220,
201,
220,
202,
200,
218,
219,
200,
219,
201,
196,
197,
218,
196,
218,
200,
217,
329,
380,
217,
380,
336,
216,
330,
329,
216,
329,
217,
215,
331,
330,
215,
330,
216,
208,
332,
331,
208,
331,
215,
214,
217,
336,
214,
336,
337,
213,
216,
217,
213,
217,
214,
212,
215,
216,
212,
216,
213,
207,
208,
215,
207,
215,
212,
211,
214,
337,
211,
337,
338,
210,
213,
214,
210,
214,
211,
209,
212,
213,
209,
213,
210,
206,
207,
212,
206,
212,
209,
199,
211,
338,
199,
338,
339,
198,
210,
211,
198,
211,
199,
197,
209,
210,
197,
210,
198,
196,
206,
209,
196,
209,
197,
195,
336,
380,
195,
380,
321,
194,
337,
336,
194,
336,
195,
193,
338,
337,
193,
337,
194,
156,
339,
338,
156,
338,
193,
192,
195,
321,
192,
321,
320,
191,
194,
195,
191,
195,
192,
190,
193,
194,
190,
194,
191,
155,
156,
193,
155,
193,
190,
189,
192,
320,
189,
320,
319,
188,
191,
192,
188,
192,
189,
187,
190,
191,
187,
191,
188,
154,
155,
190,
154,
190,
187,
159,
189,
319,
159,
319,
318,
158,
188,
189,
158,
189,
159,
157,
187,
188,
157,
188,
158,
147,
154,
187,
147,
187,
157,
186,
370,
384,
186,
384,
342,
185,
369,
370,
185,
370,
186,
184,
368,
369,
184,
369,
185,
153,
367,
368,
153,
368,
184,
183,
186,
342,
183,
342,
341,
182,
185,
186,
182,
186,
183,
181,
184,
185,
181,
185,
182,
152,
153,
184,
152,
184,
181,
180,
183,
341,
180,
341,
340,
179,
182,
183,
179,
183,
180,
178,
181,
182,
178,
182,
179,
151,
152,
181,
151,
181,
178,
156,
180,
340,
156,
340,
339,
155,
179,
180,
155,
180,
156,
154,
178,
179,
154,
179,
155,
147,
151,
178,
147,
178,
154,
177,
328,
383,
177,
383,
364,
176,
327,
328,
176,
328,
177,
175,
326,
327,
175,
327,
176,
150,
325,
326,
150,
326,
175,
174,
177,
364,
174,
364,
365,
173,
176,
177,
173,
177,
174,
172,
175,
176,
172,
176,
173,
149,
150,
175,
149,
175,
172,
171,
174,
365,
171,
365,
366,
170,
173,
174,
170,
174,
171,
169,
172,
173,
169,
173,
170,
148,
149,
172,
148,
172,
169,
153,
171,
366,
153,
366,
367,
152,
170,
171,
152,
171,
153,
151,
169,
170,
151,
170,
152,
147,
148,
169,
147,
169,
151,
168,
315,
379,
168,
379,
322,
167,
316,
315,
167,
315,
168,
166,
317,
316,
166,
316,
167,
159,
318,
317,
159,
317,
166,
165,
168,
322,
165,
322,
323,
164,
167,
168,
164,
168,
165,
163,
166,
167,
163,
167,
164,
158,
159,
166,
158,
166,
163,
162,
165,
323,
162,
323,
324,
161,
164,
165,
161,
165,
162,
160,
163,
164,
160,
164,
161,
157,
158,
163,
157,
163,
160,
150,
162,
324,
150,
324,
325,
149,
161,
162,
149,
162,
150,
148,
160,
161,
148,
161,
149,
147,
157,
160,
147,
160,
148,
146,
322,
379,
146,
379,
300,
145,
323,
322,
145,
322,
146,
144,
324,
323,
144,
323,
145,
107,
325,
324,
107,
324,
144,
143,
146,
300,
143,
300,
299,
142,
145,
146,
142,
146,
143,
141,
144,
145,
141,
145,
142,
106,
107,
144,
106,
144,
141,
140,
143,
299,
140,
299,
298,
139,
142,
143,
139,
143,
140,
138,
141,
142,
138,
142,
139,
105,
106,
141,
105,
141,
138,
110,
140,
298,
110,
298,
297,
109,
139,
140,
109,
140,
110,
108,
138,
139,
108,
139,
109,
98,
105,
138,
98,
138,
108,
137,
356,
383,
137,
383,
328,
136,
355,
356,
136,
356,
137,
135,
354,
355,
135,
355,
136,
104,
353,
354,
104,
354,
135,
134,
137,
328,
134,
328,
327,
133,
136,
137,
133,
137,
134,
132,
135,
136,
132,
136,
133,
103,
104,
135,
103,
135,
132,
131,
134,
327,
131,
327,
326,
130,
133,
134,
130,
134,
131,
129,
132,
133,
129,
133,
130,
102,
103,
132,
102,
132,
129,
107,
131,
326,
107,
326,
325,
106,
130,
131,
106,
131,
107,
105,
129,
130,
105,
130,
106,
98,
102,
129,
98,
129,
105,
128,
314,
382,
128,
382,
350,
127,
313,
314,
127,
314,
128,
126,
312,
313,
126,
313,
127,
101,
311,
312,
101,
312,
126,
125,
128,
350,
125,
350,
351,
124,
127,
128,
124,
128,
125,
123,
126,
127,
123,
127,
124,
100,
101,
126,
100,
126,
123,
122,
125,
351,
122,
351,
352,
121,
124,
125,
121,
125,
122,
120,
123,
124,
120,
124,
121,
99,
100,
123,
99,
123,
120,
104,
122,
352,
104,
352,
353,
103,
121,
122,
103,
122,
104,
102,
120,
121,
102,
121,
103,
98,
99,
120,
98,
120,
102,
119,
294,
378,
119,
378,
308,
118,
295,
294,
118,
294,
119,
117,
296,
295,
117,
295,
118,
110,
297,
296,
110,
296,
117,
116,
119,
308,
116,
308,
309,
115,
118,
119,
115,
119,
116,
114,
117,
118,
114,
118,
115,
109,
110,
117,
109,
117,
114,
113,
116,
309,
113,
309,
310,
112,
115,
116,
112,
116,
113,
111,
114,
115,
111,
115,
112,
108,
109,
114,
108,
114,
111,
101,
113,
310,
101,
310,
311,
100,
112,
113,
100,
113,
101,
99,
111,
112,
99,
112,
100,
98,
108,
111,
98,
111,
99,
97,
364,
383,
97,
383,
356,
96,
365,
364,
96,
364,
97,
95,
366,
365,
95,
365,
96,
58,
367,
366,
58,
366,
95,
94,
97,
356,
94,
356,
355,
93,
96,
97,
93,
97,
94,
92,
95,
96,
92,
96,
93,
57,
58,
95,
57,
95,
92,
91,
94,
355,
91,
355,
354,
90,
93,
94,
90,
94,
91,
89,
92,
93,
89,
93,
90,
56,
57,
92,
56,
92,
89,
61,
91,
354,
61,
354,
353,
60,
90,
91,
60,
91,
61,
59,
89,
90,
59,
90,
60,
49,
56,
89,
49,
89,
59,
88,
371,
384,
88,
384,
370,
87,
372,
371,
87,
371,
88,
86,
373,
372,
86,
372,
87,
55,
374,
373,
55,
373,
86,
85,
88,
370,
85,
370,
369,
84,
87,
88,
84,
88,
85,
83,
86,
87,
83,
87,
84,
54,
55,
86,
54,
86,
83,
82,
85,
369,
82,
369,
368,
81,
84,
85,
81,
85,
82,
80,
83,
84,
80,
84,
81,
53,
54,
83,
53,
83,
80,
58,
82,
368,
58,
368,
367,
57,
81,
82,
57,
82,
58,
56,
80,
81,
56,
81,
57,
49,
53,
80,
49,
80,
56,
79,
363,
385,
79,
385,
377,
78,
362,
363,
78,
363,
79,
77,
361,
362,
77,
362,
78,
52,
360,
361,
52,
361,
77,
76,
79,
377,
76,
377,
376,
75,
78,
79,
75,
79,
76,
74,
77,
78,
74,
78,
75,
51,
52,
77,
51,
77,
74,
73,
76,
376,
73,
376,
375,
72,
75,
76,
72,
76,
73,
71,
74,
75,
71,
75,
72,
50,
51,
74,
50,
74,
71,
55,
73,
375,
55,
375,
374,
54,
72,
73,
54,
73,
55,
53,
71,
72,
53,
72,
54,
49,
50,
71,
49,
71,
53,
70,
350,
382,
70,
382,
357,
69,
351,
350,
69,
350,
70,
68,
352,
351,
68,
351,
69,
61,
353,
352,
61,
352,
68,
67,
70,
357,
67,
357,
358,
66,
69,
70,
66,
70,
67,
65,
68,
69,
65,
69,
66,
60,
61,
68,
60,
68,
65,
64,
67,
358,
64,
358,
359,
63,
66,
67,
63,
67,
64,
62,
65,
66,
62,
66,
63,
59,
60,
65,
59,
65,
62,
52,
64,
359,
52,
359,
360,
51,
63,
64,
51,
64,
52,
50,
62,
63,
50,
63,
51,
49,
59,
62,
49,
62,
50,
48,
335,
381,
48,
381,
307,
47,
334,
335,
47,
335,
48,
46,
333,
334,
46,
334,
47,
9,
332,
333,
9,
333,
46,
45,
48,
307,
45,
307,
306,
44,
47,
48,
44,
48,
45,
43,
46,
47,
43,
47,
44,
8,
9,
46,
8,
46,
43,
42,
45,
306,
42,
306,
305,
41,
44,
45,
41,
45,
42,
40,
43,
44,
40,
44,
41,
7,
8,
43,
7,
43,
40,
12,
42,
305,
12,
305,
304,
11,
41,
42,
11,
42,
12,
10,
40,
41,
10,
41,
11,
0,
7,
40,
0,
40,
10,
39,
321,
380,
39,
380,
329,
38,
320,
321,
38,
321,
39,
37,
319,
320,
37,
320,
38,
6,
318,
319,
6,
319,
37,
36,
39,
329,
36,
329,
330,
35,
38,
39,
35,
39,
36,
34,
37,
38,
34,
38,
35,
5,
6,
37,
5,
37,
34,
33,
36,
330,
33,
330,
331,
32,
35,
36,
32,
36,
33,
31,
34,
35,
31,
35,
32,
4,
5,
34,
4,
34,
31,
9,
33,
331,
9,
331,
332,
8,
32,
33,
8,
33,
9,
7,
31,
32,
7,
32,
8,
0,
4,
31,
0,
31,
7,
30,
300,
379,
30,
379,
315,
29,
299,
300,
29,
300,
30,
28,
298,
299,
28,
299,
29,
3,
297,
298,
3,
298,
28,
27,
30,
315,
27,
315,
316,
26,
29,
30,
26,
30,
27,
25,
28,
29,
25,
29,
26,
2,
3,
28,
2,
28,
25,
24,
27,
316,
24,
316,
317,
23,
26,
27,
23,
27,
24,
22,
25,
26,
22,
26,
23,
1,
2,
25,
1,
25,
22,
6,
24,
317,
6,
317,
318,
5,
23,
24,
5,
24,
6,
4,
22,
23,
4,
23,
5,
0,
1,
22,
0,
22,
4,
21,
301,
378,
21,
378,
294,
20,
302,
301,
20,
301,
21,
19,
303,
302,
19,
302,
20,
12,
304,
303,
12,
303,
19,
18,
21,
294,
18,
294,
295,
17,
20,
21,
17,
21,
18,
16,
19,
20,
16,
20,
17,
11,
12,
19,
11,
19,
16,
15,
18,
295,
15,
295,
296,
14,
17,
18,
14,
18,
15,
13,
16,
17,
13,
17,
14,
10,
11,
16,
10,
16,
13,
3,
15,
296,
3,
296,
297,
2,
14,
15,
2,
15,
3,
1,
13,
14,
1,
14,
2,
0,
10,
13,
0,
13,
1 };

  //Bounding Volume Hiearchy Data
  BoundingVolumeHierarchy * BVHTree;
  BoundingVolumeHierarchy * BVHTreeBottomUp;

  //octree info
  Octree* OctreeForAllObjects;
  Octree* OctreePerObject;

  //shader used for rendering the lines
  ShaderHandle debugDrawID;
  ShaderHandle sphereDebugDrawID;

  public:
  DebugRenderingSystem(Scene & scene, int windowWidth, int windowHeight);
  ~DebugRenderingSystem();
  void Update(Scene& scene, int windowWidth, int windowHeight);

  //check for AABB
  bool isAABBOn = false;

  //check for square AABB
  bool isSquareAABBOn = false;

  //draw AABB of part of an object
  bool isSubObjectDrawOn = false;
  std::vector<glm::vec3> points = { 
  glm::vec3{.1f,.1f,.1f},
  glm::vec3{2.0f,2.0f,2.0f},
  glm::vec3{-2.0f,-2.0f,-2.0f} };

  //draw the OBJ Vertices once
  bool isBSOn = false;

  //draw BVH
  bool isBVHOn = false;
  int numberOfLevels = 0;
  bool isBVHBottomUpOn = false;
  int numberOfLevelsBUOn = 7;

  //draw Octree for all objects
  bool isOctreeOnForAllObjects = false;
  int levelForAllObjects = 0;
  bool isOctreeOnForSingularObject = false;
  int levelForOneObject = 0;

  BoundingSphere::BoundingSphereCalculationType sphereType = BoundingSphere::BoundingSphereCalculationType::Centroid;

  struct BVHDist 
  {
    BoundingVolumeHierarchy* parent;
    float distance;
  };

  void drawAABB(MeshComponentPtr mesh, Scene& scene, bool isSquareAABB);
  void drawAABB(std::vector<glm::vec3> points, Scene& scene, bool isSquareAABB);
  void drawAABB(AABB bounds, Scene& scene);
  void drawBS(MeshComponentPtr mesh, Scene& scene, BoundingSphere::BoundingSphereCalculationType type);
  void drawOctree(Octree * root, int level, Scene & scene);
  void createBVHTree(BoundingVolumeHierarchy * BVH, std::vector<MeshComponentPtr> meshes, int level);
  void createOctree(Octree * octree, std::vector<MeshComponentPtr> meshes, int level);
  void createOctree(Octree* octree, const glm::vec3* pointsForOneMesh, size_t size, int level);
  BoundingVolumeHierarchy* createBVHTreeBottomUp(std::vector<MeshComponentPtr> meshes, int level);
  BVHDist getClosestPair(std::vector<MeshComponentPtr> meshes);

};